#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QProcess>
#include <QDir>


class ClickableLabel : public QLabel
{
public:
    ClickableLabel(QWidget* parent = nullptr) : QLabel(parent) {}
    QPixmap getDesligado();
    QPixmap getLigado();

protected:
    void enterEvent(QEvent* event) override
    {
        Q_UNUSED(event);
        setCursor(Qt::PointingHandCursor);
    }

    void leaveEvent(QEvent* event) override
    {
        Q_UNUSED(event);
        unsetCursor();
    }
    void mousePressEvent(QMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton)
        {
            if (ligado)
            {
                setPixmap(image1);
                ligado = false;
                // Parar a execução do binário
                process->kill();
            }
            else
            {
                setPixmap(image2);
                ligado = true;
                QString resourcePath = ":/new/prefix1/monitor";
                QString temporaryDir = QDir::tempPath();
                QString temporaryFilePath = temporaryDir + "/monitor";
                QFile::copy(resourcePath, temporaryFilePath);

                // Tornar o binário executável
                QFile::setPermissions(temporaryFilePath, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);

                // Executar o binário
                process->start(temporaryFilePath,QStringList() << "");
            }
        }
    }

private:
    QPixmap image1{":/new/prefix1/desligado.png"};
    QPixmap image2{":/new/prefix1/ligado.png"};
    bool ligado = false;
    QProcess* process = new QProcess(this);
};

QPixmap ClickableLabel::getDesligado()
{
    return this->image1;
}

QPixmap ClickableLabel::getLigado()
{
    return this->image2;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Criação da janela principal
    QWidget window;
    window.setWindowTitle("Imagem Clicável");
    window.resize(400, 400);

    // Criação do rótulo com a imagem
    ClickableLabel label(&window);
    label.setAlignment(Qt::AlignCenter);
    label.setPixmap(label.getDesligado());
    label.setScaledContents(true);
    label.setFixedSize(300, 300);

    // Adiciona o rótulo à janela
    QVBoxLayout* layout = new QVBoxLayout(&window);
    layout->addWidget(&label);
    layout->setContentsMargins(50, 50, 50, 50);

    window.show();

    return a.exec();
}
