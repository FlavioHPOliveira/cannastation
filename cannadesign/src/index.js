// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAuth, onAuthStateChanged } from "firebase/auth"
//import { initializeApp } from "https://www.gstatic.com/firebasejs/9.6.3/firebase-app.js";
//import { getAuth, onAuthStateChanged } from "https://www.gstatic.com/firebasejs/9.6.3/firebase-auth.js";



// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyD1P_W4y62l8-OESX1jdW-ffuz4ub3KE2M",
  authDomain: "cannafirebase.firebaseapp.com",
  projectId: "cannafirebase",
  storageBucket: "cannafirebase.appspot.com",
  messagingSenderId: "262309337307",
  appId: "1:262309337307:web:2e269d47d5485d0d6314fe"
};

// Initialize Firebase
const firebaseApp = initializeApp(firebaseConfig);

const auth = getAuth(firebaseApp);

onAuthStateChanged(auth, user =>{
  if(user != null){
    console.log('logged in!')
  }else{
    console.log('no user')
  }
})