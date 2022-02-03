// Import the functions you need from the SDKs you need

const { db, auth, onAuthStateChanged, monitorAuthState, doc, getDoc, updateDoc } = require("./auth.js")


monitorAuthState().then( async  (user)=>{
  console.log("my user:",user)
  console.log("my user ID:", user.uid)

  const docRef = doc(db, "users", user.uid);
  const docSnap = await getDoc(docRef);

  if (docSnap.exists()) {
    console.log("*Product: Document data:", docSnap.data());
    // Set the "capital" field of the city 'DC'
    

  } else {
    // doc.data() will be undefined in this case
    console.log("No such document!");
  }

  const updateStationToken = async () => {
    console.log("inside updateStationToken")
    const boardToken = docSnap.data().email.substring(0,3) + user.uid.substring(0,3)
    console.log(boardToken)
    await updateDoc(docRef, {
      boardDefault: boardToken,
      boards: [boardToken]
    });

    window.location.href = 'station.html'
  }

  btnBuyStation = document.getElementById("btnBuyStation");
  btnBuyStation.addEventListener('click', updateStationToken);
  
  

})


