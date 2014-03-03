"use strict";
function myFunction() {
    alert("haha");
}

function test(button) {
    document.getElementById("bla").innerHTML = button;
    var placeHolder = document.getElementById("button");
    var myButton = document.createElement("input");
    myButton.type = "button";
    myButton.type = "submit";
    myButton.value = "Confirm";
    placeHolder.appendChild(myButton);
}