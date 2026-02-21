// gifrecorder.cpp
#include "gifrecorder.h"
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QBuffer>
#include <QImageWriter>
#include <QDebug>

GifRecorder::GifRecorder(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_plot1(nullptr)
    , m_plot2(nullptr)
    , m_recording(false)
{
    connect(m_timer, &QTimer::timeout, this, &GifRecorder::captureFrame);
}

void GifRecorder::startRecording(QWidget *plot1, QWidget *plot2, const QString &outputPath, int fps)
{
    if (m_recording) return;

    m_plot1 = plot1;
    m_plot2 = plot2;
    m_outputPath = outputPath;
    m_fps = fps;
    m_frames.clear();
    m_recording = true;

    // Проверяем и создаем директорию
    QDir dir = QFileInfo(outputPath).absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // Начинаем запись с указанным FPS
    m_timer->start(1000 / fps);

    qDebug() << "Начата запись GIF:" << outputPath << "FPS:" << fps;
}

void GifRecorder::stopRecording()
{
    if (!m_recording) return;

    m_timer->stop();
    m_recording = false;

    saveGif();

    qDebug() << "Запись GIF завершена. Кадров:" << m_frames.size();
    emit recordingFinished(m_outputPath);
}

bool GifRecorder::isRecording() const
{
    return m_recording;
}

void GifRecorder::captureFrame()
{
    if (!m_plot1 || !m_plot2) return;

    QImage frame = captureCombinedFrame();
    if (!frame.isNull()) {
        m_frames.append(frame);
        emit frameCaptured(m_frames.size());
    }
}

QImage GifRecorder::captureCombinedFrame()
{
    // Создаем изображение для двух графиков рядом
    int width = m_plot1->width() + m_plot2->width();
    int height = qMax(m_plot1->height(), m_plot2->height());

    QImage combined(width, height, QImage::Format_ARGB32);
    combined.fill(Qt::white);

    QPainter painter(&combined);

    // Захватываем первый график
    QPixmap pixmap1 = m_plot1->grab();
    painter.drawPixmap(0, 0, pixmap1);

    // Захватываем второй график
    QPixmap pixmap2 = m_plot2->grab();
    painter.drawPixmap(m_plot1->width(), 0, pixmap2);

    painter.end();

    return combined;
}

void GifRecorder::saveGif()
{
    if (m_frames.isEmpty()) {
        qDebug() << "Нет кадров для сохранения";
        return;
    }

    // Используем QImageWriter для GIF
    QImageWriter writer(m_outputPath);
    writer.setFormat("gif");
    writer.setQuality(100);

    // Устанавливаем задержку между кадрами (в миллисекундах)
    int delay = 1000 / m_fps;

    // Сохраняем каждый кадр
    for (int i = 0; i < m_frames.size(); ++i) {
        writer.setCompression(i); // 0 = без сжатия
        writer.setQuality(100);

        // Для GIF можно установить задержку через метаданные
        QImage image = m_frames[i];
        if (!writer.write(image)) {
            qDebug() << "Ошибка записи кадра" << i << ":" << writer.errorString();
        }

        // Небольшая задержка для синхронизации
        QApplication::processEvents();
    }

    qDebug() << "GIF сохранен в:" << m_outputPath;
}
