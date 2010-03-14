GatewayOrange.prototype = {
	name: function() {
		return "Orange";
	},

	id: function() {
		return "03";
	},

	sendSms: function(receipient, sender, signature, content, callbackObject) {
		var sender = new GatewaySmsSender();
		sender.sendSms(receipient, sender, signature, content, callbackObject);
	},
};

function GatewayOrange() {
	return this;
}

gatewayManager.addItem(new GatewayOrange());

GatewaySmsSender.prototype = {

	failure: function(errorMessage) {
		this.callbackObject.failure(errorMessage);
	},

	finished: function() {
		this.callbackObject.result();
	},

	sendSms: function(recipient, sender, signature, content, callbackObject) {
		this.recipient = recipient;
		this.sender = sender;
		this.signature = signature;
		this.content = content;
		this.callbackObject = callbackObject;

		if (!network) {
			this.failure("Network not available");
			return;
		}

		var formUrl = "http://sms.orange.pl/";

		this.reply = network.get(formUrl);
		this.reply.finished.connect(this, this.formReceived);
	},

	formReceived: function() {
		if (!this.reply.ok()) {
			this.failure("Cannot fetch token image");
			return;
		}

		var content = this.reply.content();
		var tokenPattern = new RegExp("rotate_token\\.aspx\\?token=([^\"]+)");
		var match = tokenPattern.exec(content);

		if (null == match) {
			this.failure("Cannot fetch token image");
			return;
		}

		this.token = match[1];
		var imageUrl = "http://sms.orange.pl/" + match[0];
		this.callbackObject.readToken(imageUrl, this, this.tokenRead);
	},

	tokenRead: function(tokenValue) {
		var url = "http://sms.orange.pl/sendsms.aspx";
		var postData = "";

		postData += "token=";
		postData += escape(this.token);
		postData += "&SENDER=";
		postData += escape(this.signature);
		postData += "&RECIPIENT=";
		postData += escape(this.recipient);
		postData += "&SHORT_MESSAGE=";
		postData += escape(this.content);
		postData += "&pass=";
		postData += escape(tokenValue);
		postData += "&CHK_RESP=";
		postData += "FALSE";
		postData += "&respInfo=";
		postData += "1";

		this.reply = network.post(url, postData);
		this.reply.finished.connect(this, this.smsSent);
	},

	smsSent: function() {
		if (!this.reply.ok()) {
			this.failure("Network error");
			return;
		}

		var content = this.reply.content();
		if (content.indexOf("wyczerpany") >= 0) {
			this.failure("You exceeded your daily limit");
		} else if (content.indexOf("Podano błędne hasło") >= 0) {
			this.failure("Text from the picture is incorrect");
		} else if (content.indexOf("Użytkownik nie ma aktywnej usługi") >= 0) {
			this.failure("The receiver has to enable SMS STANDARD service");
		} else if (content.indexOf("Twój SMS został wysłany") >= 0) {
			this.finished();
		} else if (content.indexOf("Wiadomość została pomyślnie wysłana") >= 0) {
			this.finished();
		} else {
			this.failure("Provider gateway results page looks strange. SMS was probably NOT sent.");
		}
	}

};

function GatewaySmsSender() {
	return this;
}
