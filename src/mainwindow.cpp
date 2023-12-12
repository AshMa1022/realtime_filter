#include "mainwindow.h"
#include "settings.h"
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>
#include <../projects-realtime-AshMa1022/src/utils/scenedata.h>
#include "../projects-realtime-AshMa1022/src/utils/sceneparser.h"


void MainWindow::initialize() {

    QLabel *backgroundLabel = new QLabel(this);
    QPixmap backgroundPixmap("/Users/ash/Desktop/CS1230/Realtime_filter/resources/images/birth.JPG");
    backgroundLabel->setPixmap(backgroundPixmap);
    backgroundLabel->setScaledContents(true); // Scale the image to fill the label

    // Set the geometry of the label to cover the entire MainWindow
    backgroundLabel->setGeometry(this->rect());
    this->autoFillBackground();
    realtime = new Realtime;
    aspectRatioWidget = new AspectRatioWidget(this);
    aspectRatioWidget->setAspectWidget(realtime, 3.f/4.f);
    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(aspectRatioWidget, 1);
    this->setLayout(hLayout);




    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("Courier New");
    QLabel *tesselation_label = new QLabel(); // Parameters label
    tesselation_label->setText("Tesselation");
    tesselation_label->setFont(font);
    QLabel *camera_label = new QLabel(); // Camera label
    camera_label->setText("Camera");
    camera_label->setFont(font);
    QLabel *filters_label = new QLabel(); // Filters label
    filters_label->setText("Filters");
    filters_label->setFont(font);
    filters_label->setStyleSheet("QLabel { color : white; }");
    QLabel *shader = new QLabel(); // Filters label
    shader->setText("Shader");
    shader->setFont(font);
    shader->setStyleSheet("QLabel { color : white; }");
    QLabel *effect = new QLabel(); // Filters label
    effect->setText("Effect");
    effect->setFont(font);
    effect->setStyleSheet("QLabel { color : white; }");

    font.setBold(false);




    // Create checkbox for per-pixel filter
    filter1 = new QCheckBox();
    filter1->setText(QStringLiteral("Blur"));
    filter1->setFont(font);
    filter1->setChecked(false);
    filter1->setStyleSheet("QCheckBox { color : white; }");

    // Create checkbox for kernel-based filter
    filter2 = new QCheckBox();
    filter2->setText(QStringLiteral("Pixelation"));
    filter2->setFont(font);
    filter2->setChecked(false);
    filter2->setStyleSheet("QCheckBox { color : white; }");


    filter3 = new QCheckBox();
    filter3->setText(QStringLiteral("Dither"));
    filter3->setFont(font);
    filter3->setChecked(false);
    filter3->setStyleSheet("QCheckBox { color : white; }");



    shader1 = new QCheckBox();
    shader1->setText(QStringLiteral("Cel Shading"));
    shader1->setFont(font);
    shader1->setChecked(false);
    shader1->setStyleSheet("QCheckBox { color : white; }");

    part = new QCheckBox();
    part->setText(QStringLiteral("Ribbon Fragments"));
    part->setFont(font);
    part->setChecked(false);
    part->setStyleSheet("QCheckBox { color : white; }");

    // Create file uploader for scene file
    uploadFile = new QPushButton();
    uploadFile->setFont(font);
    uploadFile->setText(QStringLiteral("Upload Scene File"));
    uploadFile->setStyleSheet("QPushButton { color : white; border: 1px solid white; }");






    vLayout->addWidget(uploadFile);
    vLayout->addWidget(shader);
    vLayout->addWidget(shader1);
    vLayout->addWidget(filters_label);
    vLayout->addWidget(filter1);
    vLayout->addWidget(filter2);
    vLayout->addWidget(filter3);
    vLayout->addWidget(effect);
    vLayout->addWidget(part);
    // Extra Credit:

    connectUIElements();
//    vLayout->addWidget(backgroundLabel);

    // Set default values for near and far planes
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectPerPixelFilter();
    connectKernelBasedFilter();
    connectUploadFile();
    connectCel();
    connectPart();
    connectDith();
}

void MainWindow::connectCel() {
    connect(shader1, &QCheckBox::clicked, this, &MainWindow::onCel);
}

void MainWindow::connectPerPixelFilter() {
    connect(filter1, &QCheckBox::clicked, this, &MainWindow::onPerPixelFilter);
}

void MainWindow::connectKernelBasedFilter() {
    connect(filter2, &QCheckBox::clicked, this, &MainWindow::onKernelBasedFilter);
}
void MainWindow::connectDith() {
    connect(filter3, &QCheckBox::clicked, this, &MainWindow::onDith);
}

void MainWindow::connectUploadFile() {
    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);
}
void MainWindow::connectPart() {
    connect(part, &QCheckBox::clicked, this, &MainWindow::onPart);
}

void MainWindow::onCel() {
    settings.cel = !settings.cel;
    realtime->settingsChanged();
}
void MainWindow::onDith() {
    settings.dith = !settings.dith;
    realtime->settingsChanged();
}
void MainWindow::onPart() {
    settings.part = !settings.part;
    realtime->settingsChanged();
}

void MainWindow::onPerPixelFilter() {
    settings.perPixelFilter = !settings.perPixelFilter;
    realtime->settingsChanged();
}

void MainWindow::onKernelBasedFilter() {
    settings.kernelBasedFilter = !settings.kernelBasedFilter;
    realtime->settingsChanged();
}

void MainWindow::onUploadFile() {
    // Get abs path of scene file
    QString configFilePath = QString::fromStdString("/Users/ash/Desktop/CS1230/Realtime_filter/birth.json");
    if (configFilePath.isNull()) {
        std::cout << "Failed to load null scenefile." << std::endl;
        return;
    }

    settings.sceneFilePath = configFilePath.toStdString();

    std::cout << "Loaded scenefile: \"" << configFilePath.toStdString() << "\"." << std::endl;
    RenderData metaData;
    bool success = SceneParser::parse(settings.sceneFilePath, metaData);

    if (!success) {
        std::cerr << "Error loading scene: \"" << settings.sceneFilePath << "\"" << std::endl;
    }
    else{settings.m_data = metaData;}
    realtime->sceneChanged();
}

//void MainWindow::onSaveImage() {
//    if (settings.sceneFilePath.empty()) {
//        std::cout << "No scene file loaded." << std::endl;
//        return;
//    }
//    std::string sceneName = settings.sceneFilePath.substr(0, settings.sceneFilePath.find_last_of("."));
//    sceneName = sceneName.substr(sceneName.find_last_of("/")+1);
//    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image"),
//                                                    QDir::currentPath()
//                                                        .append(QDir::separator())
//                                                        .append("student_outputs")
//                                                        .append(QDir::separator())
//                                                        .append("action")
//                                                        .append(QDir::separator())
//                                                        .append("required")
//                                                        .append(QDir::separator())
//                                                        .append(sceneName), tr("Image Files (*.png)"));
//    std::cout << "Saving image to: \"" << filePath.toStdString() << "\"." << std::endl;
//    realtime->saveViewportImage(filePath.toStdString());
//}



