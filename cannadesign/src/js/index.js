const { db, auth, onAuthStateChanged, monitorAuthState, doc, getDoc, updateDoc } = require("./auth.js")
//console.log("teste station db:", db)
//const user = getAuth().currentUser;
//console.log("*station.js, auth:", auth)


let profile = document.querySelector('#profile');


monitorAuthState().then( async  (user)=>{
  console.log("my user:",user)
  console.log("my user ID:", user.uid)

  const docRef = doc(db, "users", user.uid);
  const docSnap = await getDoc(docRef);

  if (docSnap.exists()) {
    console.log("*Product: Document data:", docSnap.data());
    profile.innerHTML = docSnap.data().name;
    
  } else {
    // doc.data() will be undefined in this case
    console.log("No such document!");
  }
  
})


let menu = document.querySelector('#menu-bars');
let navbar = document.querySelector('.navbar');

menu.onclick = () =>{
  menu.classList.toggle('fa-times');
  navbar.classList.toggle('active');
}

let section = document.querySelectorAll('section');
let navLinks = document.querySelectorAll('header .navbar a');
const selectPlanMonthly = document.querySelector('#selectPlanMonthly');
const selectPlanYearly = document.querySelector('#selectPlanYearly');
const myStation = document.querySelector('#myStation');

selectPlanMonthly.addEventListener('click', ()=>{
  window.location.href = 'product.html'
})
selectPlanYearly.addEventListener('click', ()=>{
  window.location.href = 'product.html'
})
myStation.addEventListener('click', ()=>{
  window.location.href = 'station.html'
})

window.onscroll = () =>{

  menu.classList.remove('fa-times');
  navbar.classList.remove('active');

  section.forEach(sec =>{

    let top = window.scrollY;
    let height = sec.offsetHeight;
    let offset = sec.offsetTop - 150;
    let id = sec.getAttribute('id');

    if(top >= offset && top < offset + height){
      navLinks.forEach(links =>{
        links.classList.remove('active');
        document.querySelector('header .navbar a[href*='+id+']').classList.add('active');
      });
    };

  });

}


// function loader(){
//   document.querySelector('.loader-container').classList.add('fade-out');
// }

// function fadeOut(){
//   setInterval(loader, 3000);
// }

// window.onload = fadeOut;