// Import the functions you need from the SDKs you need
 
const { doc, 
  setDoc, 
  auth, 
  logout,
  signInWithEmailAndPassword,
  createUserWithEmailAndPassword,
  db } = require("./auth.js")

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
    window.location.href = 'station.html'
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

    window.location.href = 'station.html'

   
  }catch(e){
    console.log(e)
    alert(e)
  }
}

const logoutModal = async () => {
  try{
    const userLogout = await logout()
    console.log(userLogout)
    window.location.href = 'index.html'
  }catch(e){
    console.log(e)
    alert(e)
  }
}

btnLog.addEventListener('click', loginEmailPassword)
btnSignUp.addEventListener('click', createAccount)
btnLogout.addEventListener('click',logoutModal)

