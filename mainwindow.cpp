#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGraphicsScene>
#include <QMessageBox>
//#include <QStandardItemModel>
 #include <QTableWidgetItem>

#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/ofstd/offile.h>
#include <dcmtk/dcmjpeg/djdecode.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->ui->centralwidget->setLayout(this->ui->verticalLayout);
	// Register JPEG decompression codecs
	DJDecoderRegistration::registerCodecs();
}

MainWindow::~MainWindow()
{
	// Deregister JPEG decompression codecs
	DJDecoderRegistration::cleanup();
	delete ui;
}

// adapted from https://forum.qt.io/topic/160683/how-to-load-a-dicom-data-with-dcmtk
QImage convertToQImage(DicomImage *dicomImageVal) {
	if (!dicomImageVal || dicomImageVal->getStatus() != EIS_Normal) {
		qWarning("Failed to load DICOM image.");
		return QImage();
	}

	const int width = dicomImageVal->getWidth();
	const int height = dicomImageVal->getHeight();

	// Ensure valid dimensions
	if (width <= 0 || height <= 0) {
		qWarning("Invalid DICOM dimensions.");
		return QImage();
	}

	QImage image(width, height, QImage::Format_RGB888);

	// Handle monochrome images
	if (dicomImageVal->isMonochrome()) {
		dicomImageVal->setMinMaxWindow(); // Adjust brightness and contrast
		const uchar *pixelData = (uchar *)dicomImageVal->getOutputData(8); // 8 bits per sample
		if (pixelData) {
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					int value = pixelData[y * width + x];
					image.setPixelColor(x, y, QColor(value, value, value));
				}
			}
		}
	} else {
		// Handle color images (e.g., RGB)
		const uchar *pixelData = (uchar *)dicomImageVal->getOutputData(24); // 24 bits per sample
		if (pixelData) {
			memcpy(image.bits(), pixelData, width * height * 3); // Copy RGB data directly
		}
	}

	return image;
}

double last_zoom = 1.0;

void MainWindow::on_actionAbrir_triggered()
{
	QString initialSearchDir;

	auto fileName = QFileDialog::getOpenFileName(this, tr("Open Document"), initialSearchDir, tr("DICOM files (*.dcm *.dicom)"));
	auto byteArray = fileName.toUtf8();
	std::string file_name(byteArray.constData(), byteArray.size());
	auto of_file_name(file_name.c_str());

	DcmFileFormat fileFormat;
	auto status = fileFormat.loadFile(of_file_name);

	if (!status.good()) {
		QMessageBox::critical(this, "Error", tr("Cannot read DICOM file (%1)").arg(status.text()));
		return;
	}

	DicomImage image(of_file_name, EXS_Unknown);

	auto qImage = convertToQImage(&image);
	auto pixmap = QPixmap::fromImage(qImage);
	auto *graphic = new QGraphicsScene(this);
	graphic->addPixmap(pixmap);
	graphic->setSceneRect(pixmap.rect());
	this->ui->graphicsView->setScene(graphic);
	this->ui->graphicsView->fitInView(pixmap.rect(), Qt::KeepAspectRatio);

	auto num_images = image.getFrameCount();

	for (u_int i = 0; i < num_images; i++) {
		this->ui->cbImageIndex->addItem(QString::number(i));
	}

	this->ui->cbImageIndex->setCurrentIndex(0);

	auto dataset = fileFormat.getDataset();

	if (dataset != nullptr) {
		std::array list{
			DCM_PatientName,
			DCM_StudyDate,
			DCM_StudyDescription,
			DCM_PatientSex,
			DCM_AcquisitionDeviceProcessingDescription,
			DCM_PhotometricInterpretation
		};

		auto row = 0;

		for (const auto &dcm_tag_key : list) {
			QTableWidgetItem *qtwiItem = this->ui->tableWidget->item(row++, 0);

			if (qtwiItem) {
				const char *str;
				auto condition = dataset->findAndGetString(dcm_tag_key, str);

				if(condition.good() && str != nullptr) {
					qtwiItem->setText(str);
				}
			}
		}

	}

	auto qWith = this->width();
	last_zoom = ((double)qWith / (double)qImage.width()) * 100.0;
	this->ui->horizontalSlider->setMinimum(last_zoom);
	this->ui->horizontalSlider->setValue(last_zoom);
	this->ui->graphicsView->installEventFilter(this);
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
	if (value == last_zoom) {
		return;
	}

	qreal zoom = (qreal) value / (qreal) last_zoom;
	this->ui->graphicsView->scale(zoom, zoom);
	this->ui->sbZoom->setValue(value);
	//fprintf(stdout, "last_zoom : %d, new_zoom : %d, zoom : %d\n", (int)last_zoom, value, (int)(zoom*100.0));
	//fflush(stdout);
	last_zoom = value;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::Resize) {
		if (obj == this->ui->graphicsView) {
			this->ui->graphicsView->fitInView(this->ui->graphicsView->sceneRect(), Qt::KeepAspectRatio);
			return true; // Event handled
		}
	}

	return QMainWindow::eventFilter(obj, event); // Let base class handle other events
}
