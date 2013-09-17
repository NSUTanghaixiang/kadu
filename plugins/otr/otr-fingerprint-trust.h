/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OTR_FINGERPRINT_TRUST_H
#define OTR_FINGERPRINT_TRUST_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

class Contact;
class OtrContextConverter;

class OtrFingerprintTrust : public QObject
{
	Q_OBJECT

	QWeakPointer<OtrContextConverter> ContextConverter;

public:
	enum Trust
	{
		TrustNotVerified,
		TrustVerified
	};

	explicit OtrFingerprintTrust(QObject *parent = 0);
	virtual ~OtrFingerprintTrust();

	void setContextConverter(OtrContextConverter *contextConverter);

	void setContactFingerprintTrust(const Contact &contact, Trust trust) const;
	Trust contactFingerprintTrust(const Contact &contact) const;

};

#endif // OTR_FINGERPRINT_TRUST_H