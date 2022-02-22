const { db, monitorAuthState, doc, getDoc } = require("./auth.js")

monitorAuthState().then( async  (user)=>{
  
  console.log("login_modal. my user:",user)
  
  ////////////LOGIN, LOGOUT MODAL//////////
  // Get the modal
  var loginModal = document.getElementById("loginModal");
  var logoutModal = document.getElementById("logoutModal");
  var spanLogout = document.getElementsByClassName("close")[2];
  // Get the button that opens the modal
  var btnProfileAndIcon = document.getElementById("profileAndIcon");
  //TODO , CHANGE THIS FOR REAL VALIDATION IS USER IS LOGGED IN.
  var profileLabel = document.getElementById("profile").innerHTML;
  // Get the <span> element that closes the modal
  var spanLogin = document.getElementsByClassName("close")[0];
  //var spanLight = document.getElementById("closeLightModal")
  // When the user clicks the button, open the modal 
  btnProfileAndIcon.onclick = function() {
    console.log(profileLabel)
    //if there is an user logged in..
    if(user){
      logoutModal.style.display = "block";
      console.log(profileLabel, "logged in...")
    }else{
      loginModal.style.display = "block";
      console.log(profileLabel, "NOT logged in..")
    }
  }
  // When the user clicks on <span> (x), close the modal
  spanLogin.onclick = function() {
    loginModal.style.display = "none";
  }

  spanLogout.onclick = function() {
    console.log('clicked on span register')
    logoutModal.style.display = "none";
  }

  ////////////REGISTER MODAL//////////
  // Get the modal
  var registerModal = document.getElementById("registerModal");
  var createAccount = document.getElementById("createAccount");
  var spanRegister = document.getElementsByClassName("close")[1];
  createAccount.onclick = function() {
    loginModal.style.display = "none";
    registerModal.style.display = "block";
  }
  spanRegister.onclick = function() {
    console.log('clicked on span register')
    registerModal.style.display = "none";
  }

  // When the user clicks anywhere outside of the modal, close it
  window.onclick = function(event) {
    if (event.target == registerModal) {
      registerModal.style.display = "none";
    }else if(event.target == loginModal){
      loginModal.style.display = "none";
    }
    else if(event.target == logoutModal){
      logoutModal.style.display = "none";
    }
  }



})
