#pragma once
#include <QString>

const QString buttonStyle = R"(
    QPushButton {
        background-color: #ffb347;
        border: none;
        color: white;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 4px 2px;
        transition-duration: 0.4s;
        cursor: pointer;
        border-radius: 15px;
    }
    QPushButton:hover {
        background-color: #ff7a19; 
        color: white;
    }
)";

const QString labelStyle = R"(
    QLabel {
        font-size: 16px;
        font-family: 'Georgia', serif;
        font-weight: normal;
        text-align: center;
        color: #333333; /* Dark text color */
        padding: 15px;
        border-radius: 10px; /* Rounded corners */
        text-decoration: underline;
    }
)";

const QString labelPlayerStyle = R"(
    QLabel {
        font-size: 20px;
        font-family: 'Georgia', serif;
        font-weight: normal;
        text-align: center;
        color: #333333; /* Dark text color */
        padding: 15px;
        border-radius: 10px; /* Rounded corners */
        margins 10px;
    }
)";

const QString labelStyleRed = R"(
    QLabel {
        font-size: 16px;
        font-family: 'Georgia', serif;
        font-weight: normal;
        text-align: center;
        color: #ff0000; /* Dark text color */
        padding: 15px;
        border-radius: 10px; /* Rounded corners */
        text-decoration: underline;
    }
)";

const QString ComboBoxStyle = R"(
QComboBox {
    background-color: #ffb347;
    border: 1px solid #cccccc;
    border-radius: 9px;
    padding: 2px 10px;
    font-size: 14px;
    color: #ffffff;
}

QComboBox QAbstractItemView {
    border: 1px solid #ffffff;
    selection-background-color: #f0f0f0;
    selection-color: #ffb347;
    background-color: #ffb347;
    color: #ffffff; /* Ensures the text is white */
}

QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 20px;
    border-left: 1px solid #ffffff;
}

QComboBox::down-arrow {
    image: url(:/icons/down_arrow.png); /* Use your own icon here */
    width: 10px;
    height: 10px;
}

QComboBox::item {
    color: #ffffff; /* Ensures the text is white */
}
)";


