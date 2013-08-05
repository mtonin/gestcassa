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

#ifndef QPICTUREBUTTON_H
#define QPICTUREBUTTON_H

#include <QtGui>
#include <QtDesigner/QDesignerExportWidget>


// Text offset when pushed
const int PICTURE_BUTTON_TEXT_OFFSET_PUSHED_TOP  = 1;
const int PICTURE_BUTTON_TEXT_OFFSET_PUSHED_LEFT = 1;

// Height
const int PICTURE_BUTTON_HEIGHT_MINIMUM = 10;
const int PICTURE_BUTTON_HEIGHT_DEFAULT = 30;

// Width
const int PICTURE_BUTTON_WIDTH_DEFAULT  = 50;
const int PICTURE_BUTTON_WIDTH_MINIMUM  = 20;

// Colors defines
const QColor PICTURE_BUTTON_GREY1(0xa0, 0xa0, 0xa0);  // 160
const QColor PICTURE_BUTTON_GREY2(0xb0, 0xb0, 0xb0);  // 176
const QColor PICTURE_BUTTON_GREY3(0xc0, 0xc0, 0xc0);  // 192
const QColor PICTURE_BUTTON_GREY4(0xd0, 0xd0, 0xd0);  // 208

// Default text
const char   PICTURE_BUTTON_DEFAULT_TEXT[] = "PictureButton";
const QColor PICTURE_BUTTON_DEFAULT_TEXT_COLOR_ENABLED  = Qt::black;
const QColor PICTURE_BUTTON_DEFAULT_TEXT_COLOR_DISABLED = PICTURE_BUTTON_GREY1;

// Default border
const bool   PICTURE_BUTTON_DEFAULT_BORDER_ENABLED = true;
const QColor PICTURE_BUTTON_DEFAULT_BORDER_COLOR   = Qt::black;

// Default colors
const QColor PICTURE_BUTTON_DEFAULT_COLOR_NORMAL        = PICTURE_BUTTON_GREY3;
const QColor PICTURE_BUTTON_DEFAULT_COLOR_PUSHED        = PICTURE_BUTTON_GREY1;
const QColor PICTURE_BUTTON_DEFAULT_COLOR_HOT           = PICTURE_BUTTON_GREY4;
const QColor PICTURE_BUTTON_DEFAULT_COLOR_CHECKEDHOT    = PICTURE_BUTTON_GREY2;
const QColor PICTURE_BUTTON_DEFAULT_COLOR_DISABLED      = PICTURE_BUTTON_GREY4;

// Default pictures
#define PICTURE_BUTTON_DEFAULT_PICTURE_NORMAL       NULL
#define PICTURE_BUTTON_DEFAULT_PICTURE_PUSHED       NULL
#define PICTURE_BUTTON_DEFAULT_PICTURE_HOT          NULL
#define PICTURE_BUTTON_DEFAULT_PICTURE_CHECKEDHOT   NULL
#define PICTURE_BUTTON_DEFAULT_PICTURE_DISABLED     NULL

// Default position of the icon
#define PICTURE_BUTTON_DEFAULT_ICON_POSITION PositionLeft

// Spacing
const int PICTURE_BUTTON_DEFAULT_ICON_SPACE = 5;
const int PICTURE_BUTTON_DEFAULT_TEXT_SPACE = 5;


/*
    Class that defines a picture button widget. 
    The button's look is customizable with images or fore- and background colors.
*/
class  QPictureButton : public QAbstractButton
{
    // Button properties
    Q_OBJECT

    // Enum for position top, bottom, left, right
    Q_ENUMS(EButtonPosition)

    // Text color
    Q_PROPERTY(QColor textColorEnabled READ textColorEnabled WRITE SetTextColorEnabled RESET ResetTextColorEnabled);

    // Text color (disabled)
    Q_PROPERTY(QColor textColorDisabled READ textColorDisabled WRITE SetTextColorDisabled RESET ResetTextColorDisabled);

    // Enable the border that surrounds the button.
    Q_PROPERTY(bool borderEnabled READ borderEnabled WRITE SetBorderEnabled RESET ResetBorderEnabled);

    // Color of the border that surrounds the button.
    Q_PROPERTY(QColor borderColor READ borderColor WRITE SetBorderColor RESET ResetBorderColor);

    // Button color if no picture was set.
    Q_PROPERTY(QColor buttonColorNormal READ buttonColorNormal WRITE SetButtonColorNormal RESET ResetButtonColorNormal);

    // Button color in pushed state if no picture was set.
    Q_PROPERTY(QColor buttonColorPushed READ buttonColorPushed WRITE SetButtonColorPushed RESET ResetButtonColorPushed);

    // Button color when the mouse is over and no picture was set.
    Q_PROPERTY(QColor buttonColorHot READ buttonColorHot WRITE SetButtonColorHot RESET ResetButtonColorHot);

    // Button color in checked state when the mouse is over and no picture was set.
    Q_PROPERTY(QColor buttonColorCheckedHot READ buttonColorCheckedHot WRITE SetButtonColorCheckedHot RESET ResetButtonColorCheckedHot);

    // Button color in disabled state if no picture was set.
    Q_PROPERTY(QColor buttonColorDisabled READ buttonColorDisabled WRITE SetButtonColorDisabled RESET ResetButtonColorDisabled);

    // Default picture.
    Q_PROPERTY(QPixmap pictureNormal READ pictureNormal WRITE SetPictureNormal RESET ResetPictureNormal);

    // Picture that will be displayed when the button is pushed.
    Q_PROPERTY(QPixmap picturePushed READ picturePushed WRITE SetPicturePushed RESET ResetPicturePushed);

    // Picture that will be displayed when the mouse is over the button.
    Q_PROPERTY(QPixmap pictureHot READ pictureHot WRITE SetPictureHot RESET ResetPictureHot);

    // Picture that will be displayed in the checked state when the mouse is over the button.
    Q_PROPERTY(QPixmap pictureCheckedHot READ pictureCheckedHot WRITE SetPictureCheckedHot RESET ResetPictureCheckedHot);

    // Picture that will be display when the button is disabled.
    Q_PROPERTY(QPixmap pictureDisabled READ pictureDisabled WRITE SetPictureDisabled RESET ResetPictureDisabled);

    // Position of the icon (top, bottom, left, right).
    Q_PROPERTY(EButtonPosition iconPosition READ iconPosition WRITE SetIconPosition RESET ResetIconPosition);

    // Space between the button border and the icon.
    Q_PROPERTY(int iconSpace READ iconSpace WRITE SetIconSpace RESET ResetIconSpace);

    // Space between the the icon and the button text.
    Q_PROPERTY(int textSpace READ textSpace WRITE SetTextSpace RESET ResetTextSpace);


public:

    QPictureButton(QWidget *parent = 0);
    virtual ~QPictureButton();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    // Defines for icon positions
    enum EButtonPosition
    {
        PositionTop = 0,
        PositionBottom,
        PositionLeft,
        PositionRight
    };

    // Property getter
    QColor textColorEnabled() const;
    QColor textColorDisabled() const;
    bool borderEnabled() const;
    QColor borderColor() const;
    QColor buttonColorNormal() const;
    QColor buttonColorPushed() const;
    QColor buttonColorHot() const;
    QColor buttonColorCheckedHot() const;
    QColor buttonColorDisabled() const;
    QPixmap pictureNormal() const;
    QPixmap picturePushed() const;
    QPixmap pictureHot() const;
    QPixmap pictureCheckedHot() const;
    QPixmap pictureDisabled() const;
    EButtonPosition iconPosition() const;
    int iconSpace() const;
    int textSpace() const;

public slots:

    // Property setter
    void SetTextColorEnabled(QColor);
    void SetTextColorDisabled(QColor);
    void SetBorderEnabled(bool);
    void SetBorderColor(QColor);
    void SetButtonColorNormal(QColor);
    void SetButtonColorPushed(QColor);
    void SetButtonColorHot(QColor);
    void SetButtonColorCheckedHot(QColor);
    void SetButtonColorDisabled(QColor);
    void SetPictureNormal(QPixmap);
    void SetPicturePushed(QPixmap);
    void SetPictureHot(QPixmap);
    void SetPictureCheckedHot(QPixmap);
    void SetPictureDisabled(QPixmap);
    void SetIconPosition(EButtonPosition);
    void SetIconSpace(int);
    void SetTextSpace(int);

    // Property resetter
    void ResetTextColorEnabled();
    void ResetTextColorDisabled();
    void ResetBorderEnabled();
    void ResetBorderColor();
    void ResetButtonColorNormal();
    void ResetButtonColorPushed();
    void ResetButtonColorHot();
    void ResetButtonColorCheckedHot();
    void ResetButtonColorDisabled();
    void ResetPictureNormal();
    void ResetPicturePushed();
    void ResetPictureHot();
    void ResetPictureCheckedHot();
    void ResetPictureDisabled();
    void ResetIconPosition();
    void ResetIconSpace();
    void ResetTextSpace();

protected:

    // Call for painting
    void paintEvent(QPaintEvent*);

    // Detect the mouse over the button
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

    // Paint the button
    void PaintButton();
    void PaintIcon();
    void PaintText();

private:

    // Mouse over state
    bool m_Focus;

    // Property member
    QColor m_TextColorEnabled;
    QColor m_TextColorDisabled;
    bool m_BorderEnabled;
    QColor m_BorderColor;
    QColor m_ButtonColorNormal;
    QColor m_ButtonColorPushed;
    QColor m_ButtonColorHot;
    QColor m_ButtonColorCheckedHot;
    QColor m_ButtonColorDisabled;
    QPixmap m_PictureNormal;
    QPixmap m_PicturePushed;
    QPixmap m_PictureHot;
    QPixmap m_PictureCheckedHot;
    QPixmap m_PictureDisabled;
    EButtonPosition m_IconPosition;
    int m_IconSpace;
    int m_TextSpace;
};

#endif // QPICTUREBUTTON_H
