// Import the functions you need from the SDKs you need
 
const { db } = require("./auth.js")
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
  connectFirestoreEmulator,
  query,
  where

} from "firebase/firestore";

console.log("teste 2 db:", db)

