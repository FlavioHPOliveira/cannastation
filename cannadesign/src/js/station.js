// Import the functions you need from the SDKs you need

const { db, auth, onAuthStateChanged, monitorAuthState, doc, getDoc, updateDoc } = require("./auth.js")

console.log("teste station db:", db)
//const user = getAuth().currentUser;
console.log("*station.js, auth:", auth)
//const user = auth.currentUser;

// let myUser = null
// const getUserData = async () =>{
  
//   if(myUser){
//     return myUser
//   }
//   myUser = await monitorAuthState();
//   return myUser;
// }
// console.log("my user:", getUserData())

monitorAuthState().then( async  (user)=>{
  console.log("my user:",user)
  console.log("my user ID:", user.uid)

  const docRef = doc(db, "users", user.uid);
  const docSnap = await getDoc(docRef);

  if (docSnap.exists()) {
    console.log("Document data:", docSnap.data());
    document.getElementById("userName").innerHTML = docSnap.data().name;  

  } else {
    // doc.data() will be undefined in this case
    console.log("No such document!");
  }

  ///MAIN JS
  
  // auth().onAuthStateChanged(async (user) => {
  //       // console.log('< FETCH USER MIDDLEWARE > ', user)
  //       if (user) resolve(user);
  //       if (!user) resolve({});
  //     });
    //console.log("user station.html", auth.currentUser)
    //var token = localStorage.getItem("token");
    //console.log("The Value Received is " + token);

    ////////////////////////////////SOCKET STUFF//////////////////////////////////
    // Create WebSocket connection.
    
    //const socket = new WebSocket(`ws://localhost:3000/?token=${docSnap.data().boardDefault}?clientType=app`);
    const socket = new WebSocket(`wss://cannastation.herokuapp.com/?token=${docSnap.data().boardDefault}?clientType=app`);
    console.log(socket)

    const btnLightManual    = document.querySelector("#controlLight")  
    const btnFanManual      = document.querySelector("#controlFan")
    const btnExhaustManual  = document.querySelector("#controlExhaust")
    const btnWaterManual    = document.querySelector("#controlWater")
    const btnAutoLight      = document.querySelector("#auto_light_apply")
    const btnAutoFan        = document.querySelector("#auto_fan_apply")
    const btnAutoExhaust    = document.querySelector("#auto_exhaust_apply")
    const btnAutoWater      = document.querySelector("#auto_water_apply")

    const GPIO_LIGHT   = 5
    const GPIO_FAN     = 4
    const GPIO_EXHAUST = 0
    const GPIO_WATER   = 2

    // Connection opened
    socket.addEventListener('open', function (event) {
        console.log('Connected to WS Server')
    });

    // Listen for messages
    socket.addEventListener('message', function (event) {
        console.log('Message from server ', event.data);

        try{
          const sensorData = JSON.parse(event.data)
          if( sensorData?.type == "sensor" ){
            document.getElementById("temperature").innerHTML = sensorData?.temperature; 
            document.getElementById("airHumidity").innerHTML = sensorData?.airHumidity;
            document.getElementById("soilMoisture").innerHTML = sensorData?.soilMoisture;
            document.getElementById("lastUpdate").innerHTML = sensorData?.dateTime;
            document.getElementById("boardStatusOnOff").innerHTML = "Online";


            const boardStatusIcon = document.getElementById("boardStatusOnOffIcon");
            boardStatusIcon.classList.remove("color__red")
            boardStatusIcon.classList.add("color__green")

          }else if( sensorData?.type == "controlState" ){
            
            //SET UP Cards classes
            //LIGHT
            //function approach...not sure need this.
            setLightCardStatus(sensorData?.lightOn)

            //FAN
            if( sensorData?.fanOn == 1 ){
              fanIcon.classList.remove('color__green')
              fanIcon.classList.add('color__red')
              fanControlCard.classList.remove('control__on')
              fanControlCard.classList.add('control__off')
              fanPowerIcon.classList.remove('color__green')
              fanPowerIcon.classList.add('color__red')
              document.getElementById("fanOnOffDesc").innerHTML = "Offline";

            }else if(sensorData?.fanOn == 0){
              fanIcon.classList.remove('color__red')
              fanIcon.classList.add('color__green')
              fanControlCard.classList.remove('control__off')
              fanControlCard.classList.add('control__on')
              fanPowerIcon.classList.remove('color__red')
              fanPowerIcon.classList.add('color__green')
              document.getElementById("fanOnOffDesc").innerHTML = "Online";
             
            }

            //EXHAUST
            if( sensorData?.exhaustOn == 1 ){
              exhaustIcon.classList.remove('color__green')
              exhaustIcon.classList.add('color__red')
              exhaustControlCard.classList.remove('control__on')
              exhaustControlCard.classList.add('control__off')
              exhaustPowerIcon.classList.remove('color__green')
              exhaustPowerIcon.classList.add('color__red')
              document.getElementById("exhaustOnOffDesc").innerHTML = "Offline";

            }else if(sensorData?.exhaustOn == 0){
              exhaustIcon.classList.remove('color__red')
              exhaustIcon.classList.add('color__green')
              exhaustControlCard.classList.remove('control__off')
              exhaustControlCard.classList.add('control__on')
              exhaustPowerIcon.classList.remove('color__red')
              exhaustPowerIcon.classList.add('color__green')
              document.getElementById("exhaustOnOffDesc").innerHTML = "Online";
              
            }

            //WATER
            if( sensorData?.waterOn == 1 ){
              waterIcon.classList.remove('color__green')
              waterIcon.classList.add('color__red')
              waterControlCard.classList.remove('control__on')
              waterControlCard.classList.add('control__off')
              waterPowerIcon.classList.remove('color__green')
              waterPowerIcon.classList.add('color__red')
              document.getElementById("waterOnOffDesc").innerHTML = "Offline";

            }else if(sensorData?.waterOn == 0){
              waterIcon.classList.remove('color__red')
              waterIcon.classList.add('color__green')
              waterControlCard.classList.remove('control__off')
              waterControlCard.classList.add('control__on')
              waterPowerIcon.classList.remove('color__red')
              waterPowerIcon.classList.add('color__green')
              document.getElementById("waterOnOffDesc").innerHTML = "Online";
            }

            //Remove loading and Make cards visible.
            document.getElementById("controlLoading").style.display = "none";
            const controlMainCards = document.getElementById("controlMainCards");
            controlMainCards.classList.remove("control__cards__hidden")

          }
          
        }catch(e){
          console.log(e)
        }

    });

    const sendMessage = () => {
        socket.send('Hello From Client1!');
    }

    const sendMessageON = () => {
        socket.send('ON');
    }
    const sendMessageOFF = () => {
        socket.send('OFF');
    }
    //////////////////////////////// END OF SOCKET STUFF//////////////////////////////////

    //I am going to use function or just put inside message? wait...
    const setLightCardStatus = (onOff) => {
      if(onOff == 1){
          lightIcon.classList.remove('color__green')
          lightIcon.classList.add('color__red')
          lightControlCard.classList.remove('control__on')
          lightControlCard.classList.add('control__off')
          lightPowerIcon.classList.remove('color__green')
          lightPowerIcon.classList.add('color__red')
          document.getElementById("lightOnOffDesc").innerHTML = "Offline";
      }else if(onOff == 0){
          lightIcon.classList.remove('color__red')
          lightIcon.classList.add('color__green')
          lightControlCard.classList.remove('control__off')
          lightControlCard.classList.add('control__on')
          lightPowerIcon.classList.remove('color__red')
          lightPowerIcon.classList.add('color__green')
          document.getElementById("lightOnOffDesc").innerHTML = "Online";

      }
    }
    //////////////////////////////// LIGHT CONTROL //////////////////////////////////
    const sendMessageLight = () => {

        const lightButton      = document.getElementById("controlLight")
        const lightIcon        = document.getElementById("lightIcon")
        const lightControlCard = document.getElementById("lightControlCard")
        const lightPowerIcon   = document.getElementById("lightPowerIcon")

        console.log(lightButton.value);
        let lightControlMessage = ''

        //Sending GPIO Inverting 0 and 1 because for the board 0 means ON, and 1 means OFF.
        if(lightButton.value == 1){
          lightControlMessage = `{"type":"control", "control":"light", "GPIO":${GPIO_LIGHT}, "OnOff":1}`
          lightButton.value = 0

        }
        else if(lightButton.value == 0){
          lightControlMessage = `{"type":"control", "control":"light", "GPIO":${GPIO_LIGHT}, "OnOff":0}`
          lightButton.value = 1
        }

        socket.send(lightControlMessage);
    }
    btnLightManual.addEventListener('click', sendMessageLight)

    //////////////////////////////// END OF LIGHT CONTROL //////////////////////////////////

    //////////////////////////////// FAN CONTROL //////////////////////////////////
    const sendMessageFan = () => {

    const fanButton      = document.getElementById("controlFan")
    const fanIcon        = document.getElementById("fanIcon")
    const fanControlCard = document.getElementById("fanControlCard")
    const fanPowerIcon   = document.getElementById("fanPowerIcon")

    console.log(fanButton.value);
    let fanControlMessage = ''

    //Sending GPIO Inverting 0 and 1 because for the board 0 means ON, and 1 means OFF.
    if(fanButton.value == 1){
      fanControlMessage = `{"type":"control", "control":"fan", "GPIO":${GPIO_FAN}, "OnOff":1}`
      fanButton.value = 0
    }
    else if(fanButton.value == 0){
      fanControlMessage = `{"type":"control", "control":"fan", "GPIO":${GPIO_FAN}, "OnOff":0}`
      fanButton.value = 1
    }

    socket.send(fanControlMessage);
    }
    btnFanManual.addEventListener('click', sendMessageFan)
    //////////////////////////////// END OF FAN CONTROL //////////////////////////////////

   //////////////////////////////// EXHAUST CONTROL //////////////////////////////////
   const sendMessageExhaust = () => {

    const exhaustButton      = document.getElementById("controlExhaust")
    const exhaustIcon        = document.getElementById("exhaustIcon")
    const exhaustControlCard = document.getElementById("exhaustControlCard")
    const exhaustPowerIcon   = document.getElementById("exhaustPowerIcon")

    console.log(exhaustButton.value);
    let exhaustControlMessage = ''

    //Sending GPIO Inverting 0 and 1 because for the board 0 means ON, and 1 means OFF.
    if(exhaustButton.value == 1){
      exhaustControlMessage = `{"type":"control", "control":"exhaust", "GPIO":${GPIO_EXHAUST}, "OnOff":1}`
      exhaustButton.value = 0
    }
    else if(exhaustButton.value == 0){
      exhaustControlMessage = `{"type":"control", "control":"exhaust", "GPIO":${GPIO_EXHAUST}, "OnOff":0}`
      exhaustButton.value = 1
    }

    socket.send(exhaustControlMessage);
    }
    btnExhaustManual.addEventListener('click', sendMessageExhaust)
    //////////////////////////////// END OF EXHAUST CONTROL //////////////////////////////////

    //////////////////////////////// WATER CONTROL //////////////////////////////////
    const sendMessageWater = () => {

    const waterButton       = document.getElementById("controlWater")
    const waterIcon         = document.getElementById("waterIcon")
    const waterControlCard  = document.getElementById("waterControlCard")
    const waterPowerIcon    = document.getElementById("waterPowerIcon")

    console.log(waterButton.value);
    let waterControlMessage = ''

    //Sending GPIO Inverting 0 and 1 because for the board 0 means ON, and 1 means OFF.
    if(waterButton.value == 1){
      waterControlMessage = `{"type":"control", "control":"water", "GPIO":${GPIO_WATER}, "OnOff":1}`
      waterButton.value = 0
    }
    else if(waterButton.value == 0){
      waterControlMessage = `{"type":"control", "control":"water", "GPIO":${GPIO_WATER}, "OnOff":0}`
      waterButton.value = 1
    }

    socket.send(waterControlMessage);
    }
    btnWaterManual.addEventListener('click', sendMessageWater)
    //////////////////////////////// END OF WATER CONTROL //////////////////////////////////

    // //Water Control
    // const sendMessageWater = () => {
    // const waterCheckbox = document.getElementById("controlW ater")
    // //Inverting 0 and 1 because for the board 0 means ON, and 1 means OFF.
    // console.log('clicked checkbox',Number(!waterCheckbox.checked))
    // const waterControlMessage = `{"type":"control", "control":"water", "GPIO":${GPIO_WATER}, "OnOff":${Number(!waterCheckbox.checked)}}`

    // socket.send(waterControlMessage);

    // }

    //////////////////////////////// AUTOMATIC LIGHT CONTROL //////////////////////////////////
    const sendMessageAutoLight = () => {
      console.log('btn light auto clicked')
      const lightAutoJSON = {
        type: "control_auto",
        control: "light",
        hourOn:    parseInt(document.getElementById("hourOn").value),
        hourOff:   parseInt(document.getElementById("hourOff").value),
        minuteOn:  parseInt(document.getElementById("minuteOn").value),
        minuteOff: parseInt(document.getElementById("minuteOff").value)
      }

      const lightAutoStringfy = JSON.stringify(lightAutoJSON)
      console.log(lightAutoStringfy)
      socket.send(lightAutoStringfy);

    }
    btnAutoLight.addEventListener('click', sendMessageAutoLight)

    //////////////////////////////// AUTOMATIC FAN CONTROL //////////////////////////////////
    const sendMessageAutoFan = () => {
      console.log('btn fan auto clicked')
      const fanAutoJSON = {
        type: "control_auto",
        control: "fan",
        fanTempOn: parseInt(document.getElementById("fanTempOn").value)
      }
      const fanAutoStringfy = JSON.stringify(fanAutoJSON)
      console.log(fanAutoStringfy)
      socket.send(fanAutoStringfy);

    }
    btnAutoFan.addEventListener('click', sendMessageAutoFan)

    //////////////////////////////// AUTOMATIC EXHAUST CONTROL //////////////////////////////////
    const sendMessageAutoExhaust = () => {
      console.log('btn exhaust auto clicked')
      const exhaustAutoJSON = {
        type: "control_auto",
        control: "exhaust",
        exhaustAirHumidityOn: parseInt(document.getElementById("exhaustAirHumidityOn").value)
      }
      const exhaustAutoStringfy = JSON.stringify(exhaustAutoJSON)
      console.log(exhaustAutoStringfy)
      socket.send(exhaustAutoStringfy);

    }
    btnAutoExhaust.addEventListener('click', sendMessageAutoExhaust)

    //////////////////////////////// AUTOMATIC WATER CONTROL //////////////////////////////////
    const sendMessageAutoWater = () => {
      console.log('btn exhaust auto clicked')
      const waterAutoJSON = {
        type:    "control_auto",
        control: "water",
        waterStartingHour:    parseInt(document.getElementById("waterStartingHour").value),
        waterEveryXDay:       parseInt(document.getElementById("waterEveryXDay").value),
        waterDurationSeconds: parseInt(document.getElementById("waterDurationSeconds").value)
        
      }
      const waterAutoStringfy = JSON.stringify(waterAutoJSON)
      console.log(waterAutoStringfy)
      socket.send(waterAutoStringfy);

    }
    btnAutoWater.addEventListener('click', sendMessageAutoWater)

})

// export { sendMessageLight };

//getUserTest();

//monitorAuthStateStation();
//como funciona async await... ??? quero usar o user que for retornado da funcao
// const user = monitorAuthStateStation();
// console.log("returned user from monitorStateFuinction", user);

// auth.auth().onAuthStateChanged(function(user) {
//   if (user) {
//     console.log("*station.js, currentUser:", user)
//   } else {
//     // No user is signed in.
//   }
// });


