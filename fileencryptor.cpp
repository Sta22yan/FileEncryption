#include "fileencryptor.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextStream>
#include <QCryptographicHash>

FileEncryptor::FileEncryptor(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("File Encryptor Util");
    setFixedSize(300, 150);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    encryptButton = new QPushButton("Зашифровать файл", this);
    decryptButton = new QPushButton("Расшифровать файл", this);

    layout->addWidget(encryptButton);
    layout->addWidget(decryptButton);

    setCentralWidget(centralWidget);

    connect(encryptButton, &QPushButton::clicked, this, &FileEncryptor::encryptFile);
    connect(decryptButton, &QPushButton::clicked, this, &FileEncryptor::decryptFile);

    passwordsFile.setFileName("passwords.txt");
}

void FileEncryptor::encryptFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл для шифрования");
    if (filePath.isEmpty()) return;

    bool ok;
    QString password = QInputDialog::getText(this, "Пароль", "Придумайте пароль:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для чтения");
        return;
    }

    QByteArray fileData = inputFile.readAll();
    inputFile.close();

    QByteArray key = password.toUtf8();
    QByteArray encryptedData;
    encryptedData.resize(fileData.size());

    for (int i = 0; i < fileData.size(); ++i) {
        encryptedData[i] = fileData[i] ^ key[i % key.size()];
    }

    QString outputPath = filePath + ".enc";
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи");
        return;
    }

    outputFile.write(encryptedData);
    outputFile.close();

    if (!passwordsFile.open(QIODevice::Append | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить пароль");
        return;
    }

    QTextStream out(&passwordsFile);
    out << outputPath << ":" << password << "\n";
    passwordsFile.close();

    QMessageBox::information(this, "Успех", "Зашифрованный файл создан");
}

void FileEncryptor::decryptFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл для расшифровки");
    if (filePath.isEmpty()) return;

    bool ok;
    QString password = QInputDialog::getText(this, "Пароль", "Введите пароль:", QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    bool passwordCorrect = false;

    QFile passwordsFile("passwords.txt");
    if (passwordsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&passwordsFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            int lastColonPos = line.lastIndexOf(':');
            if (lastColonPos != -1) {
                QString savedFilePath = line.left(lastColonPos);
                QString savedPassword = line.mid(lastColonPos + 1);

                if (savedFilePath == filePath && savedPassword == password) {
                    passwordCorrect = true;
                    break;
                }
            }
        }
        passwordsFile.close();
    }

    if (!passwordCorrect) {
        QMessageBox::critical(this, "Ошибка", "Пароль неверный или не найден для этого файла");
        return;
    }

    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для чтения");
        return;
    }

    QByteArray encryptedData = inputFile.readAll();
    inputFile.close();

    QByteArray key = password.toUtf8();
    QByteArray decryptedData;
    decryptedData.resize(encryptedData.size());

    for (int i = 0; i < encryptedData.size(); ++i) {
        decryptedData[i] = encryptedData[i] ^ key[i % key.size()];
    }

    QString outputPath = filePath;
    if (filePath.endsWith(".enc")) {
        outputPath = filePath.left(filePath.length() - 4);
    } else {
        outputPath += ".dec";
    }

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи");
        return;
    }

    outputFile.write(decryptedData);
    outputFile.close();

    QMessageBox::information(this, "Успех", "Файл успешно расшифрован");
}
