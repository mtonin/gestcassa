/*
    QPictureButton (QtPlusExtension) - graphical button widget

    Copyright (C) 2008 Martin Gaus (Martin.Gaus@itpo.de)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

    MIT License
*/

#include "QPictureButton.h"

QPictureButton::QPictureButton(QWidget* parent) : QAbstractButton(parent)
{
    // Set defaults
    m_Focus                 = false;

    m_TextColorEnabled      = PICTURE_BUTTON_DEFAULT_TEXT_COLOR_ENABLED;
    m_TextColorDisabled     = PICTURE_BUTTON_DEFAULT_TEXT_COLOR_DISABLED;
    m_BorderEnabled         = PICTURE_BUTTON_DEFAULT_BORDER_ENABLED;
    m_BorderColor           = PICTURE_BUTTON_DEFAULT_BORDER_COLOR;
    m_ButtonColorNormal     = PICTURE_BUTTON_DEFAULT_COLOR_NORMAL;
    m_ButtonColorPushed     = PICTURE_BUTTON_DEFAULT_COLOR_PUSHED;
    m_ButtonColorHot        = PICTURE_BUTTON_DEFAULT_COLOR_HOT;
    m_ButtonColorCheckedHot = PICTURE_BUTTON_DEFAULT_COLOR_CHECKEDHOT;
    m_ButtonColorDisabled   = PICTURE_BUTTON_DEFAULT_COLOR_DISABLED;
    m_PictureNormal         = PICTURE_BUTTON_DEFAULT_PICTURE_NORMAL;
    m_PicturePushed         = PICTURE_BUTTON_DEFAULT_PICTURE_PUSHED;
    m_PictureHot            = PICTURE_BUTTON_DEFAULT_PICTURE_HOT;
    m_PictureCheckedHot     = PICTURE_BUTTON_DEFAULT_PICTURE_CHECKEDHOT;
    m_PictureDisabled       = PICTURE_BUTTON_DEFAULT_PICTURE_DISABLED;
    m_IconPosition          = PICTURE_BUTTON_DEFAULT_ICON_POSITION;
    m_IconSpace             = PICTURE_BUTTON_DEFAULT_ICON_SPACE;
    m_TextSpace             = PICTURE_BUTTON_DEFAULT_TEXT_SPACE;

    setText(PICTURE_BUTTON_DEFAULT_TEXT);
}

QPictureButton::~QPictureButton() 
{
}

// Paint the button
void QPictureButton::paintEvent(QPaintEvent*)
{
    PaintButton();
    PaintIcon();
    PaintText();
}

void QPictureButton::PaintButton()
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect Rect = painter.window();

    QPixmap CurrentPixmap;
    QColor CurrentButtonColor;
    if (this->isEnabled())
    {
        if (m_Focus == true)
        {
            if(this->isCheckable())
            {
                if(this->isChecked() || this->isDown())
                {
                    CurrentPixmap      = m_PictureCheckedHot;
                    CurrentButtonColor = m_ButtonColorCheckedHot;
                }
                else
                {
                    CurrentPixmap      = m_PictureHot;
                    CurrentButtonColor = m_ButtonColorHot;
                } // if(m_Checked)
            }
            else
            {
                if (this->isDown())
                {
                    CurrentPixmap      = m_PicturePushed;
                    CurrentButtonColor = m_ButtonColorPushed;
                }
                else
                {
                    CurrentPixmap      = m_PictureHot;
                    CurrentButtonColor = m_ButtonColorHot;
                } // if (m_Checked == true)
            } // if(m_Checkable && m_Checked)
        }
        else
        {
            if(this->isCheckable())
            {
                if(this->isChecked() || this->isDown())
                {
                    CurrentPixmap      = m_PicturePushed;
                    CurrentButtonColor = m_ButtonColorPushed;
                }
                else
                {
                    CurrentPixmap      = m_PictureNormal;
                    CurrentButtonColor = m_ButtonColorNormal;
                } // if(m_Checked)
            }
            else
            {
                if (this->isDown())
                {
                    CurrentPixmap      = m_PictureNormal;
                    CurrentButtonColor = m_ButtonColorNormal;
                }
                else
                {
                    CurrentPixmap      = m_PictureNormal;
                    CurrentButtonColor = m_ButtonColorNormal;
                } // if (m_Checked == true)
            } // if(m_Checkable && m_Checked)
        } // if (m_Focus == true)
    }
    else
    {
        CurrentPixmap      = m_PictureDisabled;
        CurrentButtonColor = m_ButtonColorDisabled;
    } // if (this->isEnabled())

    if(CurrentPixmap.isNull())
    {
        if(m_BorderEnabled)
        {
            painter.setPen(QPen(m_BorderColor));
        }
        else
        {
            painter.setPen(Qt::NoPen);
        }
        QBrush Brush(CurrentButtonColor, Qt::SolidPattern);
        painter.setBrush(Brush);
        painter.drawRect(Rect);
    }
    else
    {
        painter.drawPixmap(Rect, CurrentPixmap);

        if(m_BorderEnabled)
        {
            painter.setPen(QPen(m_BorderColor));
            QBrush Brush(CurrentButtonColor, Qt::NoBrush);
            painter.setBrush(Brush);
            painter.drawRect(Rect);
        }
    }
}

void QPictureButton::PaintIcon()
{
    if(!icon().isNull())
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        QRect PainterRect = painter.window();

        int Top  = (PainterRect.height() - iconSize().height()) / 2;
        int Left = (PainterRect.width()  - iconSize().width())  / 2;

        switch(m_IconPosition)
        {
            case PositionTop:
                Top = m_IconSpace;
                break;
            case PositionBottom:
                Top = PainterRect.height() - iconSize().height() - m_IconSpace;
                break;
            case PositionLeft:
                Left = m_IconSpace;
                break;
            case PositionRight:
                Left = PainterRect.width() - iconSize().width() - m_IconSpace;
                break;
        }

        QPixmap Pixmap = icon().pixmap(iconSize());
        QRect PixmapRect(Left, Top, iconSize().width(), iconSize().height());

        painter.drawPixmap(PixmapRect, Pixmap);
    }
}

void QPictureButton::PaintText()
{
    // No need to set the font, default font property is used (QWidget)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect PainterRect = painter.window();

    // Text position
    int Top    = PainterRect.top();
    int Left   = PainterRect.left();
    int Height = PainterRect.height();
    int Width  = PainterRect.width();
    int Align = Qt::AlignCenter;

    if(!this->icon().isNull())
    {
        switch(m_IconPosition)
        {
            case PositionTop:
                Top += (this->iconSize().height() + m_IconSpace + m_TextSpace);
                Align = Qt::AlignHCenter | Qt::AlignTop;
                break;
            case PositionBottom:
                Top -= (this->iconSize().height() + m_IconSpace + m_TextSpace);
                Align = Qt::AlignHCenter | Qt::AlignBottom;
                break;
            case PositionLeft:
                Left += (this->iconSize().width() + m_IconSpace + m_TextSpace);
                Align = Qt::AlignLeft | Qt::AlignVCenter;
                break;
            case PositionRight:
                Left -= (this->iconSize().width() + m_IconSpace + m_TextSpace);
                Align = Qt::AlignRight | Qt::AlignVCenter;
                break;
        }
    }

    if((this->isDown()) ||(this->isChecked()))
    {
        Top  += PICTURE_BUTTON_TEXT_OFFSET_PUSHED_TOP;
        Left += PICTURE_BUTTON_TEXT_OFFSET_PUSHED_LEFT;
    }

    // Text color
    if(this->isEnabled())
    {
        painter.setPen(QPen(m_TextColorEnabled));
    }
    else
    {
        painter.setPen(QPen(m_TextColorDisabled));
    }

    // Paint
    QRect FontRect(Left, Top, Width, Height);
    painter.drawText(FontRect, Align, this->text());
}

// Property "textColorEnabled"
QColor QPictureButton::textColorEnabled() const
{
    return m_TextColorEnabled;
}

void QPictureButton::SetTextColorEnabled(QColor TextColorEnabled)
{
    m_TextColorEnabled = TextColorEnabled;
    update();
}

void QPictureButton::ResetTextColorEnabled()
{
    m_TextColorEnabled = PICTURE_BUTTON_DEFAULT_TEXT_COLOR_ENABLED;
    update();
}

// Property "textColorDisabled"
QColor QPictureButton::textColorDisabled() const
{
    return m_TextColorDisabled;
}

void QPictureButton::SetTextColorDisabled(QColor TextColorDisabled)
{
    m_TextColorDisabled = TextColorDisabled;
    update();
}

void QPictureButton::ResetTextColorDisabled()
{
    m_TextColorDisabled = PICTURE_BUTTON_DEFAULT_TEXT_COLOR_DISABLED;
    update();
}

// Property "borderEnabled"
bool QPictureButton::borderEnabled() const
{
    return m_BorderEnabled;
}

void QPictureButton::SetBorderEnabled(bool BorderEnabled)
{
    m_BorderEnabled = BorderEnabled;
    update();
}

void QPictureButton::ResetBorderEnabled()
{
    m_BorderEnabled = PICTURE_BUTTON_DEFAULT_BORDER_ENABLED;
    update();
}

// Property "borderColor"
QColor QPictureButton::borderColor() const
{
    return m_BorderColor;
}

void QPictureButton::SetBorderColor(QColor BorderColor)
{
    m_BorderColor = BorderColor;
    update();
}

void QPictureButton::ResetBorderColor()
{
    m_BorderColor = PICTURE_BUTTON_DEFAULT_BORDER_COLOR;
    update();
}

// Property "buttonColorNormal"
QColor QPictureButton::buttonColorNormal() const
{
    return m_ButtonColorNormal;
}

void QPictureButton::SetButtonColorNormal(QColor ButtonColor)
{
    m_ButtonColorNormal = ButtonColor;
    update();
}

void QPictureButton::ResetButtonColorNormal()
{
    m_ButtonColorNormal = PICTURE_BUTTON_DEFAULT_COLOR_NORMAL;
    update();
}

// Property "buttonColorPushed"
QColor QPictureButton::buttonColorPushed() const
{
    return m_ButtonColorPushed;
}

void QPictureButton::SetButtonColorPushed(QColor ButtonColor)
{
    m_ButtonColorPushed = ButtonColor;
    update();
}

void QPictureButton::ResetButtonColorPushed()
{
    m_ButtonColorPushed = PICTURE_BUTTON_DEFAULT_COLOR_PUSHED;
    update();
}

// Property "buttonColorHot"
QColor QPictureButton::buttonColorHot() const
{
    return m_ButtonColorHot;
}

void QPictureButton::SetButtonColorHot(QColor ButtonColor)
{
    m_ButtonColorHot = ButtonColor;
    update();
}

void QPictureButton::ResetButtonColorHot()
{
    m_ButtonColorHot = PICTURE_BUTTON_DEFAULT_COLOR_HOT;
    update();
}

// Property "buttonColorCheckedHot"
QColor QPictureButton::buttonColorCheckedHot() const
{
    return m_ButtonColorCheckedHot;
}

void QPictureButton::SetButtonColorCheckedHot(QColor ButtonColor)
{
    m_ButtonColorCheckedHot = ButtonColor;
    update();
}

void QPictureButton::ResetButtonColorCheckedHot()
{
    m_ButtonColorCheckedHot = PICTURE_BUTTON_DEFAULT_COLOR_CHECKEDHOT;
    update();
}

// Property "buttonColorDisabled"
QColor QPictureButton::buttonColorDisabled() const
{
    return m_ButtonColorDisabled;
}

void QPictureButton::SetButtonColorDisabled(QColor ButtonColor)
{
    m_ButtonColorDisabled = ButtonColor;
    update();
}

void QPictureButton::ResetButtonColorDisabled()
{
    m_ButtonColorDisabled = PICTURE_BUTTON_DEFAULT_COLOR_DISABLED;
    update();
}

// Property "pictureNormal"
QPixmap QPictureButton::pictureNormal() const
{
    return m_PictureNormal;
}

void QPictureButton::SetPictureNormal(QPixmap Picture)
{
    m_PictureNormal = Picture;
    update();
}

void QPictureButton::ResetPictureNormal()
{
    m_PictureNormal = PICTURE_BUTTON_DEFAULT_PICTURE_NORMAL;
    update();
}

// Property "picturePushed"
QPixmap QPictureButton::picturePushed() const
{
    return m_PicturePushed;
}

void QPictureButton::SetPicturePushed(QPixmap Picture)
{
    m_PicturePushed = Picture;
    update();
}

void QPictureButton::ResetPicturePushed()
{
    m_PicturePushed = PICTURE_BUTTON_DEFAULT_PICTURE_PUSHED;
    update();
}

// Property "pictureHot"
QPixmap QPictureButton::pictureHot() const
{
    return m_PictureHot;
}

void QPictureButton::SetPictureHot(QPixmap Picture)
{
    m_PictureHot = Picture;
    update();
}

void QPictureButton::ResetPictureHot()
{
    m_PictureHot = PICTURE_BUTTON_DEFAULT_PICTURE_HOT;
    update();
}

// Property "pictureCheckedHot"
QPixmap QPictureButton::pictureCheckedHot() const
{
    return m_PictureCheckedHot;
}

void QPictureButton::SetPictureCheckedHot(QPixmap Picture)
{
    m_PictureCheckedHot = Picture;
    update();
}

void QPictureButton::ResetPictureCheckedHot()
{
    m_PictureCheckedHot = PICTURE_BUTTON_DEFAULT_PICTURE_CHECKEDHOT;
    update();
}

// Property "pictureDisabled"
QPixmap QPictureButton::pictureDisabled() const
{
    return m_PictureDisabled;
}

void QPictureButton::SetPictureDisabled(QPixmap Picture)
{
    m_PictureDisabled = Picture;
    update();
}

void QPictureButton::ResetPictureDisabled()
{
    m_PictureDisabled = PICTURE_BUTTON_DEFAULT_PICTURE_DISABLED;
    update();
}

// Property "iconPosition"
QPictureButton::EButtonPosition QPictureButton::iconPosition() const
{
    return m_IconPosition;
}

void QPictureButton::SetIconPosition(EButtonPosition Position)
{
    m_IconPosition = Position;
    update();
}

void QPictureButton::ResetIconPosition()
{
    m_IconPosition = PICTURE_BUTTON_DEFAULT_ICON_POSITION;
    update();
}

// Property "iconSpace"
int QPictureButton::iconSpace() const
{
    return m_IconSpace;
}

void QPictureButton::SetIconSpace(int Space)
{
    m_IconSpace = Space;
    update();
}

void QPictureButton::ResetIconSpace()
{
    m_IconSpace = PICTURE_BUTTON_DEFAULT_ICON_SPACE;
    update();
}

// Property "textSpace"
int QPictureButton::textSpace() const
{
    return m_TextSpace;
}

void QPictureButton::SetTextSpace(int Space)
{
    m_TextSpace = Space;
    update();
}

void QPictureButton::ResetTextSpace()
{
    m_TextSpace = PICTURE_BUTTON_DEFAULT_TEXT_SPACE;
    update();
}

// Property "minimumSizeHint" overwritten
QSize QPictureButton::minimumSizeHint() const
{
    return QSize(PICTURE_BUTTON_WIDTH_MINIMUM, PICTURE_BUTTON_HEIGHT_MINIMUM);
}

// Property "sizeHint" overwritten
QSize QPictureButton::sizeHint() const
{
    return QSize(PICTURE_BUTTON_WIDTH_DEFAULT, PICTURE_BUTTON_HEIGHT_DEFAULT);
}


// Detect the mouse over the button
void QPictureButton::enterEvent(QEvent *)
{
    m_Focus = true;
    update();
}

void QPictureButton::leaveEvent(QEvent *)
{
    m_Focus = false;
    update();
}
