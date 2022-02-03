// Import the functions you need from the SDKs you need
 
const { doc, 
  setDoc, 
  auth, 
  logout,
  signInWithEmailAndPassword,
  createUserWithEmailAndPassword,
  db } = require("./auth.js")

// import { initializeApp } from "firebase/app";
// import { getAuth, 
//   connectAuthEmulator, 
//   signInWithEmailAndPassword, 
//   createUserWithEmailAndPassword,
//   onAuthStateChanged,
//   signOut
// } from "firebase/auth";

// import { getFirestore,
//   collection, 
//   addDoc,
//   getDocs,
//   connectFirestoreEmulator,
//   query,
//   where

// } from "firebase/firestore";

console.log("teste 2 db:", db)

// //import { initializeApp } from "https://www.gstatic.com/firebasejs/9.6.3/firebase-app.js";
// //import { getAuth, onAuthStateChanged } from "https://www.gstatic.com/firebasejs/9.6.3/firebase-auth.js";
// // TODO: Add SDKs for Firebase products that you want to use
// // https://firebase.google.com/docs/web/setup#available-libraries


const btnLog = document.querySelector("#btnLogin")
const btnSignUp = document.querySelector("#btnSignUp")
const btnLogout = document.querySelector("#btnLogout")
// //console.log(btnLog)

const loginEmailPassword = async () => {
  const loginEmail = document.querySelector("#txtEmail").value;
  const loginPwd = document.querySelector("#txtPwd").value;
  try{
    const userCredential = await signInWithEmailAndPassword(auth, loginEmail, loginPwd)
    console.log(userCredential.user)
  }catch(e){
    console.log(e)
    alert(e)
  }
}

const createAccount = async () =>{
  const loginEmail = document.querySelector("#txtEmail").value;
  const loginPwd = document.querySelector("#txtPwd").value;
  const name = document.querySelector("#txtName").value;
  try{
    const userCredential = await createUserWithEmailAndPassword(auth, loginEmail, loginPwd)
    console.log(userCredential.user)
    //localStorage.setItem("user", userCredential.user);

    try {
      //TODO
      //Create mechanism to generate tokens when user buys a board.
      //const boardToken = userCredential.user.email.substring(0,3) + userCredential.user.uid.substring(0,3)
      //console.log("boardToken: ", boardToken)
      const docRef = await setDoc(doc(db, "users", userCredential.user.uid), {
        name: name,
        email: userCredential.user.email,
        uID: userCredential.user.uid,
        boardDefault: null,
        boards: []
      });
      console.log("New Station Doc Inserted: ", docRef.id);
    } catch (e) {
      console.error("Error adding document: ", e);
    }

    window.location.href = 'product.html'

    //TODO
    //salvar ussuario na collection do usuario, com todos usuarios que vem do 
   
  }catch(e){
    console.log(e)
    alert(e)
  }
}

btnLog.addEventListener('click', loginEmailPassword)
btnSignUp.addEventListener('click', createAccount)

// const monitorAuthState = async () => {
//   onAuthStateChanged(auth, user => {
//     //if user signs in... open app
//     if(user){
//       console.log(user);
//       console.log(user.uid)
//       alert('you are logged in.')
//       //window.location = 'http://localhost:5501/cannadesign/dist/station.html'
//       //show application
//       //showLoginState(user)
//       //hideLoginError()
//       //const myuser = auth.currentUser;
//       setTokenLocalStorage(user.uid);
//       //localStorage.setItem("token", token);
//       //window.location = 'http://localhost:5501/cannadesign/dist/station.html'
//       // if (myuser) {
//       //   // User is signed in, see docs for a list of available properties
//       //   // https://firebase.google.com/docs/reference/js/firebase.User
//       //   console.log(myuser.displayName)
//       //   // ...
//       // } else {
//       //   console.log("no user is signed in. monitor auth state")
//       // }
  

//     }
//     //if user signs out, show register form.
//     else{
//       //show login form
//       alert('you are logged out.')
//     }
//   })
// }

// monitorAuthState();

// const logout = async () =>{
//   await signOut(auth);
//   //alert('you are signOut')
// }

btnLogout.addEventListener('click',logout)

// const setTokenLocalStorage = async (uid) =>{
  
//   const q = query(collection(db, "station"), where("uID", "==", uid));
//   const querySnapshot = await getDocs(q);
//   let token = ""
//   console.log('here')
//   querySnapshot.forEach((doc) => {
//     // doc.data() is never undefined for query doc snapshots
//     //console.log(doc.id, " => ", doc.data());
//     //console.log(doc.id, " => ", doc.data().token);
//     token = doc.data().token
//   });

//   localStorage.setItem("token", token);

// }

