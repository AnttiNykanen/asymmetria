var ds = $('#ds');
var dv = $('#dv');
var on = $('#on');
var off = $('#off');

var poll_ival = 2000;

var uiv;

function setUiv() {
    uiv = setInterval(upd, poll_ival);
}

function clearUiv() {
    if (uiv != null) {
        clearInterval(uiv);
    }
}

var upd = function() {
    var r = new j();
    r.open("GET", "/getDimmer", true);
    r.onreadystatechange = function() {
        if (r.readyState == 4) {
            if (r.status == 200) {
                d = JSON.parse(r.responseText);
                on.disabled = (d.status == "on");
                off.disabled = (d.status == "off");
                dv.innerHTML = ds.value = d.dimValue;
            } else {
                clearUiv();
            }
        }
    };
    r.send();
};

ds.onmousedown = function(e) {
    clearUiv();
};

on.onmousedown = ds.onmousedown;
off.onmousedown = ds.onmousedown;

ds.onmouseup = function(e) {
    var val = ds.value;
    dv.innerHTML = val;

    var r = new j();
    r.open("GET", "/setDimmer?dimValue="+val, true);
    r.send();

    setUiv();
};

function setStatus(s) {
    var r = new j();
    r.open("GET", "/setStatus?status="+s, true);
    r.send();
}

on.onmouseup = function(e) {
    setStatus('on');
    on.disabled = true;
    off.disabled = false;
    setUiv();
}

off.onmouseup = function(e) {
    setStatus('off');
    off.disabled = true;
    on.disabled = false;
    setUiv();
}

$('#logout').onclick = function(e) {
    var r = new j();
    r.open("GET", "/", true);
    r.setRequestHeader("Authorization", "Basic" + btoa("incorrect:incorrect"));
    r.send();
}

document.addEventListener("DOMContentLoaded", function(e) {
    upd();
    setUiv();
});
