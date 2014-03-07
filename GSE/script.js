"use strict";
function confirmCommand(button) {
    document.getElementById("bla").innerHTML = button;
    var placeHolder = document.getElementById("button");
    var myButton = document.createElement("input");
    /*myButton.type = "button";*/
    myButton.type = "submit";
    myButton.value = "Confirm";
    myButton.disabled = false;
    myButton.onclick = function () {sendCommand(button); };
    if (!placeHolder.childNodes[3]) {
        placeHolder.appendChild(myButton);
    } else {
        placeHolder.replaceChild(myButton, placeHolder.childNodes[3]);
    }
}

function sendCommand(button) {
    alert(button);
}