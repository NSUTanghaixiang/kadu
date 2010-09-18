#ifndef SMS_H
#define SMS_H

#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtNetwork/QHttp>

#include "buddies/buddy-manager.h"
#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "gui/actions/action.h"
#include "gui/windows/main-configuration-window.h"

#include "sms_exports.h"

class QCheckBox;
class QComboBox;
class QListWidget;
class QProcess;
class QTextEdit;

class SMSAPI SmsConfigurationUiHandler : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	QCheckBox *useBuiltIn;
	QLineEdit *customApp;
	QCheckBox *useCustomString;
	QLineEdit *customString;
	QListWidget *gatewayListWidget;

	void createDefaultConfiguration();

private slots:
	void mainConfigurationWindowDestroyed();

protected:
	virtual void configurationUpdated();

public:
	SmsConfigurationUiHandler();
	virtual ~SmsConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public slots:
	void onSmsBuildInCheckToggle(bool);
	void onUpButton();
	void onDownButton();
};

extern SMSAPI SmsConfigurationUiHandler *smsConfigurationUiHandler;

#endif
