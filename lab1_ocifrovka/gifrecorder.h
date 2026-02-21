// gifrecorder.h
#ifndef GIFRECORDER_H
#define GIFRECORDER_H

#include <QObject>
#include <QTimer>
#include <QImage>
#include <QDir>
#include <QPainter>

class GifRecorder : public QObject
{
    Q_OBJECT
public:
    explicit GifRecorder(QObject *parent = nullptr);

    void startRecording(QWidget *plot1, QWidget *plot2, const QString &outputPath, int fps = 10);
    void stopRecording();
    bool isRecording() const;

signals:
    void recordingFinished(const QString &path);
    void frameCaptured(int frameNumber);

private slots:
    void captureFrame();

private:
    QTimer *m_timer;
    QList<QImage> m_frames;
    QWidget *m_plot1;
    QWidget *m_plot2;
    QString m_outputPath;
    int m_fps;
    bool m_recording;

    QImage captureCombinedFrame();
    void saveGif();
};

#endif // GIFRECORDER_H
