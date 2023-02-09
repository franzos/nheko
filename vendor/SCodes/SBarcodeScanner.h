#ifndef SBARCODESCANNER_H
#define SBARCODESCANNER_H

#include <QObject>
#include <QCamera>
#include <QThread>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include "SBarcodeDecoder.h"

class Worker;

/*!
 * \brief The SBarcodeScanner class is a custom class that allows image processing with the cooperation of QML VideoOutput type.
 */
class SBarcodeScanner : public QVideoSink
{
    Q_OBJECT

    Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)
    Q_PROPERTY(QRectF captureRect READ captureRect WRITE setCaptureRect NOTIFY captureRectChanged)

public:
    explicit SBarcodeScanner(QObject *parent = nullptr);
    ~SBarcodeScanner() override;

    SBarcodeDecoder *getDecoder() ;

    /*!
     * \fn QVideoSink *videoSink() const
     * \brief Function for getting sink of video output
     */
    QVideoSink *videoSink() const;

    /*!
     * \fn void setVideoSink(QVideoSink *videoSink)
     * \brief Function for setting sink of video output
     * \param const QVideoSink *videoSink - video sink
     */
    void setVideoSink(QVideoSink *videoSink);

    /*!
     * \fn QRectF captureRect() const
     * \brief Function for getting capture area
     */
    QRectF captureRect() const;

    /*!
     * \fn void setCaptureRect(const QRectF &captureRect)
     * \brief Function for setting capture area
     * \param const QRectF &captureRect - capture area
     */
    void setCaptureRect(const QRectF &captureRect);

    /*!
     * \fn QString captured() const
     * \brief Function for getting captured string
     */
    QString captured() const;

public slots:
    /*!
     * \fn void pauseProcessing()
     * \brief Function for pause image processing
     */
    void pauseProcessing();

    /*!
     * \fn void continueProcessing()
     * \brief Function for continue image processing
     */
    void continueProcessing();

    /*!
     * \fn void setProcessing(bool p)
     * \brief Function for setting process status
     */
    void setProcessing(bool p);

    /*!
     * \fn void imageProcess(const QVideoFrame &frame)
     * \brief Function for image processing
     * \param const QVideoFrame &frame - video frame
     */
    void imageProcess(SBarcodeDecoder *decoder, const QImage &image, ZXing::BarcodeFormats formats);

private:
    /*!
     * \brief Decoder instance
     */
    SBarcodeDecoder m_decoder;

    /*!
     * \brief Camera instance
     */
    QCamera *camera;

    /*!
     * \brief Pointer to a sink
     */
    QPointer<QVideoSink> m_videoSink;

    /*!
     * \brief Capture area
     */
    QRectF m_captureRect;

    /*!
     * \brief Captured string
     */
    QString m_captured = "";

    /*!
     * \brief Camera session instance
     */
    QMediaCaptureSession m_capture;

    /*!
     * \brief An instance of a thread
     */
    QThread workerThread;

    /*!
     * \brief A pointer of a Worker class
     */
    Worker *worker;

    /*!
     * \brief Indicates the processing state
     */
    bool m_processing = true;

    /*!
     * \fn void setCaptured(const QString &captured)
     * \brief Function for setting capture string
     * \param const QString &captured - captured string
     */
    void setCaptured(const QString &captured);

    /*!
     * \fn void handleFrameCaptured(const QVideoFrame &frame)
     * \brief Function for handling video frame
     * \param const QVideoFrame &frame - video frame
     */
    void handleFrameCaptured(const QVideoFrame &frame);

signals:
    /*!
     * \brief This signal emitted when camera changed
     */
    void cameraChanged();

    /*!
     * \brief This signal emitted for running process in a thread
     */
    void process(const QImage &image);

    /*!
     * \brief This signal is emitted when sink changed
     */
    void videoSinkChanged();

    /*!
     * \brief This signal is emitted when capture area changed
     * \param const QRectF &captureRect - capture area
     */
    void captureRectChanged(const QRectF &captureRect);

    /*!
     * \brief This signal is emitted when captured string changed
     * \param const QString &captured - captured string
     */
    void capturedChanged(const QString &captured);

private slots:
    /*!
     * \fn void initCam()
     * \brief Function for initialization of camera
     */
    void initCam();

    /*!
     * \fn void stopCam()
     * \brief Function for stopping camera
     */
    void stopCam();
};

/*!
 * \brief The Worker class allows you to run `SBarcodeScanner::imageProcess` method in a different thread
 */
class Worker : public QObject
{
    Q_OBJECT

private:
    /*!
     * \brief A pointer of SBarcodeScanner
     */
    SBarcodeScanner *_scanner;

public:
    /*!
     * \fn Worker(SBarcodeScanner *scanner)
     * \brief Constructor.
     * \param SBarcodeScanner *_scanner - a pointer to scanner class.
     */
    Worker(SBarcodeScanner *scanner) : _scanner{scanner} { ; }

public slots:

    /*!
    * \fn void process(const QImage &image)
    * \param const QImage &image - captured image
    * \brief An interface for processing image
    */
    void process(const QImage &image) {
        _scanner->imageProcess(_scanner->getDecoder(), image, SCodes::toZXingFormat(SCodes::SBarcodeFormat::Basic));
    }

};

#endif // SBARCODESCANNER_H
