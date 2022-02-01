////////////LIGHT MODAL//////////
// Get the modal
var lightModal = document.getElementById("lightModal");
// Get the button that opens the modal
var btnAutomaticLight = document.getElementById("automaticLight");
// Get the <span> element that closes the modal
var spanLight = document.getElementsByClassName("close")[0];
//var spanLight = document.getElementById("closeLightModal")
// When the user clicks the button, open the modal 
btnAutomaticLight.onclick = function() {
  lightModal.style.display = "block";
}
// When the user clicks on <span> (x), close the modal
spanLight.onclick = function() {
  lightModal.style.display = "none";
}
// When the user clicks anywhere outside of the modal, close it
window.onclick = function(event) {
  if (event.target == lightModal) {
    lightModal.style.display = "none";
  }
}

////////////FAN MODAL//////////
// Get the modal
var fanModal = document.getElementById("fanModal");
// Get the button that opens the modal
var btnAutomaticFan = document.getElementById("automaticFan");
// Get the <span> element that closes the modal
var spanFan = document.getElementsByClassName("close")[1];
// When the user clicks the button, open the modal 
btnAutomaticFan.onclick = function() {
  fanModal.style.display = "block";
}
// When the user clicks on <span> (x), close the modal
spanFan.onclick = function() {
  fanModal.style.display = "none";
}
// When the user clicks anywhere outside of the modal, close it
window.onclick = function(event) {
  if (event.target == fanModal) {
    fanModal.style.display = "none";
  }
}



////////////EXHAUST MODAL//////////
// Get the modal
var exhaustModal = document.getElementById("exhaustModal");
// Get the button that opens the modal
var btnAutomaticExhaust = document.getElementById("automaticExhaust");
// Get the <span> element that closes the modal
var spanExhaust = document.getElementsByClassName("close")[2];
// When the user clicks the button, open the modal 
btnAutomaticExhaust.onclick = function() {
  exhaustModal.style.display = "block";
}
// When the user clicks on <span> (x), close the modal
spanExhaust.onclick = function() {
  exhaustModal.style.display = "none";
}
// When the user clicks anywhere outside of the modal, close it
window.onclick = function(event) {
  if (event.target == exhaustModal) {
    exhaustModal.style.display = "none";
  }
}


////////////WATER MODAL//////////
// Get the modal
var waterModal = document.getElementById("waterModal");
// Get the button that opens the modal
var btnAutomaticWater = document.getElementById("automaticWater");
// Get the <span> element that closes the modal
var spanWater = document.getElementsByClassName("close")[3];
// When the user clicks the button, open the modal 
btnAutomaticWater.onclick = function() {
  waterModal.style.display = "block";
}
// When the user clicks on <span> (x), close the modal
spanWater.onclick = function() {
  waterModal.style.display = "none";
}

// When the user clicks anywhere outside of the modal, close it
window.onclick = function(event) {
  if (event.target == lightModal ||
      event.target == fanModal ||
      event.target == exhaustModal ||
      event.target == waterModal
     ) 
  {
    fanModal.style.display = "none";
    exhaustModal.style.display = "none";
    lightModal.style.display = "none";
    waterModal.style.display = "none";
  }
}