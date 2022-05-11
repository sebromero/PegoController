function sendMail(recipients, message) {
    for (let recipient of recipients) {
        MailApp.sendEmail(recipient, "🚨 Cold store alarm", message);
    }
}

function getValue(data, key) {
    for (let entry of data.values) {
        if (entry.name == key) return entry.value;
    }
    return null;
}

function doPost(e) {
    let recipients = ["your-email@email.com", "your-other-email@email.com"];
    if (e) {
        let data = JSON.parse(e.postData.contents)
        if (getValue(data, "deviceResponsive") == false) sendMail(recipients, "⚠️ The cold store controller is not responding.");
        if (getValue(data, "temperatureAlarmStatus") == true) sendMail(recipients, "⚠️ The cold store temperature alarm was triggered!");
        if (getValue(data, "openDoorAlarmStatus") == true) sendMail(recipients, "⚠️ The cold store open-door alarm was triggered!");
    }

    return ContentService.createTextOutput(JSON.stringify({ "result": "done" }));
}