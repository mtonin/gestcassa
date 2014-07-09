#ifndef REPARTOBTNWIDGET_H
#define REPARTOBTNWIDGET_H

#include <QPictureButton.h>

class RepartoBtnWidget : public QPictureButton
{
    Q_OBJECT
public:
    explicit RepartoBtnWidget(int id, QWidget *parent = 0);
    explicit RepartoBtnWidget(QWidget *parent = 0);

    const int getId() {
        return idReparto;
    }
    const QString getNomeReparto() {
        return nomeReparto;
    }
    void setNomeReparto(const QString nuovoReparto);
    const QColor getColore() {
        return buttonColorNormal();
    }
    const QColor getColoreText() {
        return textColorEnabled();
    }
    const QFont getFont() {
        return font();
    }
    bool getAbilitato() {
        return abilitato;
    }
    void setAbilitato(bool val) {
        abilitato = val;
    }

signals:
    void cambiaColore(const QColor& colore);
    void cambiaFont(const QFont& font);
    void cambiaColoreText(const QColor& colore);

public slots:
    void setColore(const QColor& colore);
    void setButtonFont(const QFont& font);
    void setColoreText(const QColor& colore);

private:
    int idReparto;
    QString nomeReparto;
    bool abilitato;

private slots:

};

#endif // REPARTOBTNWIDGET_H
