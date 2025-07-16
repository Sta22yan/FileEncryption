#ifndef FILEENCRYPTOR_H
#define FILEENCRYPTOR_H

#include <QMainWindow>
#include <QPushButton>
#include <QFile>

class FileEncryptor : public QMainWindow {
    Q_OBJECT

public:
    explicit FileEncryptor(QWidget *parent = nullptr);

private slots:
    void encryptFile();
    void decryptFile();

private:
    QPushButton *encryptButton;
    QPushButton *decryptButton;
    QFile passwordsFile;
};

#endif // FILEENCRYPTOR_H
