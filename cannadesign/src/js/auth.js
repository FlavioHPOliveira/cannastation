// Import the functions you need from the SDKs you need
import { async } from "@firebase/util";
import { initializeApp } from "firebase/app";
import { getAuth, 
  connectAuthEmulator, 
  signInWithEmailAndPassword, 
  createUserWithEmailAndPassword,
  onAuthStateChanged,
  signOut
} from "firebase/auth";

import { getFirestore,
  collection, 
  addDoc,
  getDocs,
  getDoc,
  doc,
  setDoc,
  connectFirestoreEmulator,
  query,
  where, 
  updateDoc

} from "firebase/firestore";

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
const auth = getAuth(firebaseApp)

const db = getFirestore(firebaseApp);

//connectFirestoreEmulator(db, 'localhost', 8080);
//connectAuthEmulator(auth, "http://localhost:9099");

const monitorAuthState = () => {

return new Promise(function(resolve, reject) {
  
  onAuthStateChanged(auth, user => {
    //if user signs in... open app
    if(user){
      console.log('you are logged in. User details:')
      console.log(user);
      console.log(user.uid)

      //alert('you are logged in.')
      //window.location = 'http://localhost:5501/cannadesign/dist/station.html'
      //show application
      //showLoginState(user)
      //hideLoginError()
      //const myuser = auth.currentUser;
      setTokenLocalStorage(user.uid);
      //localStorage.setItem("token", token);
      //window.location = 'http://localhost:5501/cannadesign/dist/station.html'
      // if (myuser) {
      //   // User is signed in, see docs for a list of available properties
      //   // https://firebase.google.com/docs/reference/js/firebase.User
      //   console.log(myuser.displayName)
      //   // ...
      // } else {
      //   console.log("no user is signed in. monitor auth state")
      // }
      resolve(user);
    }
    //if user signs out, show register form.
    else{
      //show login form
      if(window.location.pathname !== "/login.html"){
        window.location.href = "/login.html"
      }
      console.log('you are logged out.')
      resolve(false);
    }
  })

});

  
}

monitorAuthState();



const logout = async () =>{
  await signOut(auth);
  //alert('you are signOut')
}

const getUserTest = () =>{
  const auth = getAuth();
  const user = auth.currentUser; 
  console.log("user test", user)
}

const setTokenLocalStorage = async (uid) =>{
  
  const q = query(collection(db, "station"), where("uID", "==", uid));
  const querySnapshot = await getDocs(q);
  let token = ""
  console.log('here')
  querySnapshot.forEach((doc) => {
    // doc.data() is never undefined for query doc snapshots
    //console.log(doc.id, " => ", doc.data());
    //console.log(doc.id, " => ", doc.data().token);
    token = doc.data().token
  });

  localStorage.setItem("token", token);

}

export { doc, setDoc, collection, auth, logout, signInWithEmailAndPassword, 
         createUserWithEmailAndPassword, onAuthStateChanged, db, monitorAuthState, getDoc, updateDoc };