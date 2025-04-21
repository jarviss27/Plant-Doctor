// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
//import { getAnalytics } from "firebase/analytics";
import { getDatabase } from "firebase/database";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyA1WKY9M1IajEc00MRc5tznYdb2dt3ay6M",
  authDomain: "plant-doctor-f9ebc.firebaseapp.com",
  databaseURL: "https://plant-doctor-f9ebc-default-rtdb.firebaseio.com",
  projectId: "plant-doctor-f9ebc",
  storageBucket: "plant-doctor-f9ebc.firebasestorage.app",
  messagingSenderId: "889089321429",
  appId: "1:889089321429:web:23f630975d4f7c45c2755c",
  measurementId: "G-76PWJJFXZ6"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
//const analytics = getAnalytics(app);
const db = getDatabase(app);

export { db };