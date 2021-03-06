﻿#include "Other.h"
#include <QFile>
#include "crc32/crc32.h"


quint32 rand_u32()
{
	quint32 randValueList[] = { (quint32)rand() & 0x7FFF, (quint32)rand() & 0x7FFF, (quint32)rand() & 3 };
	return randValueList[0] | (randValueList[1] << 15) | (randValueList[2] << 30);
}

QByteArray boolToQByteArray(bool value)
{
	return value ? "1" : "0";
}

bool QByteArrayToBool(const QByteArray & value)
{
	return value.toInt() == 1;
}

QByteArray checksumThenUnpackPackage(QByteArray package)
{
	if (package.size() < 4)
		return QByteArray();
	const QByteArray content = QByteArray::fromRawData(package.constData() + 4, package.size() - 4);
	const quint32 receivedCrc = *(quint32*)package.constData();
	const quint32 actualCrc = crc32(content.constData(), content.size());
	if (receivedCrc != actualCrc)
		return QByteArray();
	return content;
}

QHostAddress tryConvertToIpv4(const QHostAddress & hostAddress)
{
	if (hostAddress.protocol() == QAbstractSocket::IPv4Protocol)
		return hostAddress;
	if (hostAddress.protocol() == QAbstractSocket::IPv6Protocol)
	{
		quint32 ipv4Address = hostAddress.toIPv4Address();
		if (ipv4Address)
			return QHostAddress(ipv4Address);
	}
	return hostAddress;
}

QString getSocketPeerDescription(const QAbstractSocket * socket)
{
	return QString("%1:%2").arg(tryConvertToIpv4(socket->peerAddress()).toString()).arg(socket->peerPort());
}

QString getNatDescription(NatType natType)
{
	switch (natType)
	{
	case UnknownNatType:
		return "UnknownNatType";
	case PublicNetwork:
		return "PublicNetwork";
	case FullOrRestrictedConeNat:
		return "FullOrRestrictedConeNat";
	case PortRestrictedConeNat:
		return "PortRestrictedConeNat";
	case SymmetricNat:
		return "SymmetricNat";
	default:
		return "Error";
	}
}

bool generalNameCheck(const QString & name)
{
	if (name.length() == 0)
		return false;

	static QString forbiddenChar = "\\/:?*\"'`<>| ,";
	for (int i = 0; i < forbiddenChar.length(); ++i)
	{
		if (name.indexOf(forbiddenChar.at(i)) >= 0)
			return false;
	}
	return true;
}

QByteArray readFile(const QString fileName)
{
	QFile file(fileName);
	if (file.open(QIODevice::ReadOnly))
		return file.readAll();
	else
		return QByteArray();
}
