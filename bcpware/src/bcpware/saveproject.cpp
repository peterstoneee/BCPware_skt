#include "saveproject.h"


SaveProject::SaveProject(QWidget *parent) :QDialog(parent)
{
	this->setWindowTitle(tr("Save Project"));
    browseBtn = createButton(tr("&Browse..."), SLOT(browse()));

    projectNameComboBox = createComboBox();
    directoryComboBox = createComboBox();

    projectName = createLabel(tr("project Name"));
    savePosition = createLabel(tr("directory"));

	ok_cancel_btn_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(ok_cancel_btn_box, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ok_cancel_btn_box, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *mainlayout = new QGridLayout;
    mainlayout->addWidget(projectName,0, 0);
    mainlayout->addWidget(projectNameComboBox,0, 1,1,2);

    mainlayout->addWidget(savePosition,1, 0);
    mainlayout->addWidget(directoryComboBox,1,1,1,2);
    mainlayout->addWidget(browseBtn,1, 3);
	mainlayout->addWidget(ok_cancel_btn_box,3,2);

    this->setLayout(mainlayout);
    resize(500, 100);
}

void SaveProject::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Dierectory"), QDir::currentPath());

    if (!directory.isEmpty()) {
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}

QComboBox *SaveProject::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}

QLabel *SaveProject::createLabel(const QString &text)
{
    QLabel *label = new QLabel;
    label->setText(text);
    return label;
}
QPushButton *SaveProject::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}
QStringList *SaveProject::returndata()
{
	QStringList *list = new QStringList;
	*list << projectNameComboBox->currentText() << directoryComboBox->currentText();
	return list;
}


SaveProject::~SaveProject(){}
