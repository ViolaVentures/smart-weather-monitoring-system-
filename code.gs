function doGet(e) { 
Logger.log( JSON.stringify(e) );
var result = 'Ok';
if (e.parameter == 'undefined') {
result = 'No Parameters';
}
else {
var sheet_id = '1g2CFFzpmTC6mc7n4v6NAiI25PThryTAOvsgRp70BDjQ'; // Spreadsheet ID
var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();
var newRow = sheet.getLastRow() + 1; 
var rowData = [];
var Curr_Date = new Date();
rowData[0] = Curr_Date; // Date in column A
var Curr_Time = Utilities.formatDate(Curr_Date, "UTC", 'HH:mm:ss');
rowData[1] = Curr_Time; // Time in column B
var Curr_TimeI = Utilities.formatDate(Curr_Date, "Asia/Kolkata", 'HH:mm:ss');
rowData[2] = Curr_TimeI; // Time in column B
for (var param in e.parameter) {
Logger.log('In for loop, param=' + param);
var value = stripQuotes(e.parameter[param]);
Logger.log(param + ':' + e.parameter[param]);
switch (param) {
case 'temperature':
rowData[3] = value; // Temperature in column C
result = 'Temperature Written on column C'; 
break;
case 'humidity':
rowData[4] = value; // Humidity in column D
result += ' ,Humidity Written on column D'; 
break; 
case 'MQ7':
rowData[5] = value; // CO in column E
result += ' ,MQ7 Written on column E'; 
break; 
case 'Rain':
rowData[6] = value; // Rain in column F
result += ' ,rain Written on column F'; 
break; 
default:
result = "unsupported parameter";
}
}
Logger.log(JSON.stringify(rowData));
var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
newRange.setValues([rowData]);
}
return ContentService.createTextOutput(result);
}
function stripQuotes( value ) {
return value.replace(/^["']|['"]$/g, "");
}