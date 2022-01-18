// Import the functions you need from the SDKs you need
import { async } from "@firebase/util";
import { initializeApp } from "firebase/app";
import { getAuth, connectAuthEmulator, signInWithEmailAndPassword, onAuthStateChanged } from "firebase/auth"
//import { initializeApp } from "https://www.gstatic.com/firebasejs/9.6.3/firebase-app.js";
//import { getAuth, onAuthStateChanged } from "https://www.gstatic.com/firebasejs/9.6.3/firebase-auth.js";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// import {
//   hideLoginError,
//   showLoginState,
//   showLoginForm,
//   showApp,
//   showLoginError,
//   btnLogin,
//   btnSignup,

// }


// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyD1P_W4y62l8-OESX1jdW-ffuz4u  b3KE2M",
  authDomain: "cannafirebase.firebaseapp.com",
  projectId: "cannafirebase",
  storageBucket: "cannafirebase.appspot.com",
  messagingSenderId: "262309337307",
  appId: "1:262309337307:web:2e269d47d5485d0d6314fe"
};


// Initialize Firebase
const firebaseApp = initializeApp(firebaseConfig);
const auth = getAuth(firebaseApp)

connectAuthEmulator(auth, "http://localhost:9099");

const btnLog = document.querySelector("#btnLogin")
console.log(btnLog)

const loginEmailPassword = async () => {
  const loginEmail = document.querySelector("#txtEmail").value;
  const loginPwd = document.querySelector("#txtPwd").value;

  const userCredential = await signInWithEmailAndPassword(auth, loginEmail, loginPwd)
  console.log(userCredential.user)

}

btnLog.addEventListener('click', loginEmailPassword)
