//program that controls and updates UI along with garage model

//variables representing elements on the login screen
var loginButton
var createAccountButton

//variables representing elements on the create account screen
var backButton

//variables representing elements on the "Basic" screen
var logOutButton
var doorStatusLabel
var doorButton
var lightStatusLabel
var lightButton
var autoCloseButton

//variables representing elements on the "Advanced" screen
var autoCloseStatusLabel
var autoCloseTimeSlider
var lightBrightnessSlider
var lightOffTimeSlider
var autoCloseTime
var lightBrightness
var lightOffTime

//when the user login successfully, show the control page
function loginClicked(event) {
  document.getElementById("loginPage").hidden = true
  document.getElementById("allControls").hidden = false;
}

//when the user hit create account, show the create account page
function createAccountClicked(event) {
  document.getElementById("loginPage").hidden = true
  document.getElementById("createAccountPage").hidden = false
}

//when the user hit back to get to the login page
function backClicked(event) {
  document.getElementById("loginPage").hidden = false
  document.getElementById("createAccountPage").hidden = true
}

//when the user log out from control page
function logOutClicked(event) {
  document.getElementById("loginPage").hidden = false
  document.getElementById("allControls").hidden = true;
}

//when the door button is clicked
function doorClicked(event) {
  garage.setDoorPowered()
}

//when the light button is clicked
function lightClicked(event) {
  garage.setLightPowered()
}

//when the auto-close is clicked
function autoCloseClicked(event) {
  garage.setAutoCloseEnabled()
}

//when the auto-close slider is adjusted
function autoCloseTimeSliderChange(event) {
  autoCloseTime.innerText = autoCloseTimeSlider.value + "s"
  garage.setDoorAutoCloseTime(autoCloseTimeSlider.value)
}

//when the light brightness slider is adjusted
function lightBrightnessSliderChange(event) {
  lightBrightness.innerText = lightBrightnessSlider.value + "%"
  garage.setLightBrightness(lightBrightnessSlider.value)
}

//when the light auto-off slider is adjusted
function lightOffTimeSliderChange(event) {
  lightOffTime.innerText = lightOffTimeSlider.value + "s"
  garage.setLightAutoOffTime(lightOffTimeSlider.value)
}

function loadingPage(value) {
  document.getElementById("allControls").hidden = value;
}

//when the garage model is updated
function stateUpdate(newState) {
  loadingPage(false);
  doorStatusLabel.innerText = newState.doorPowered
  faultStatusLabel.innerText = newState.haveFault
  autoCloseTimeSlider.value = newState.doorAutoCloseTime
  lightBrightnessSlider.value = newState.lightBrightness
  lightOffTimeSlider.value = newState.lightAutoOffTime

  if (newState.lightPowered) {
    lightStatusLabel.innerText = "On"
  } else {
    lightStatusLabel.innerText = "Off"
  }

  if (newState.autoCloseEnabled) {
    autoCloseStatusLabel.innerText = "On"
  } else {
    autoCloseStatusLabel.innerText = "Off"
  }
  autoCloseTime.innerText = autoCloseTimeSlider.value + "s"
  lightBrightness.innerText = lightBrightnessSlider.value + "%"
  lightOffTime.innerText = lightOffTimeSlider.value + "s"
}

window.addEventListener("load", function(event) {
  //login page variables
  loginButton = document.getElementById("loginButton")
  createAccountButton = document.getElementById("createAccountButton")

  //back button in create account page
  backButton = document.getElementById("backButton")


  //door and light on/off variables
  logOutButton = document.getElementById("logOutButton")
  doorButton = document.getElementById("doorButton")
  doorStatusLabel = document.getElementById("doorStatus")
  lightButton = document.getElementById("lightButton")
  lightStatusLabel = document.getElementById("lightStatus")
  autoCloseButton = document.getElementById("autoCloseButton")

  //advanced setting variables
  autoCloseStatusLabel = document.getElementById("autoCloseStatus")
  autoCloseTimeSlider = document.getElementById("autoCloseTimeSlider")
  lightBrightnessSlider = document.getElementById("lightBrightnessSlider")
  lightOffTimeSlider = document.getElementById("lightOffTimeSlider")
  autoCloseTime = document.getElementById("autoCloseTime")
  lightBrightness = document.getElementById("lightBrightness")
  lightOffTime = document.getElementById("lightOffTime")
  faultStatusLabel = document.getElementById("faultStatus")

  //login page event handlers
  // loginButton.addEventListener("click", loginClicked)
  // createAccountButton.addEventListener("click", createAccountClicked)

  //back button event handler in create account page
  // backButton.addEventListener("click", backClicked)

  //door and light on/off event handlers
  // logOutButton.addEventListener("click", logOutClicked)
  doorButton.addEventListener("click", doorClicked)
  lightButton.addEventListener("click", lightClicked)
  autoCloseButton.addEventListener("click", autoCloseClicked)

  //advanced setting event handlers
  autoCloseTimeSlider.addEventListener("change", autoCloseTimeSliderChange)
  lightBrightnessSlider.addEventListener("change", lightBrightnessSliderChange)
  lightOffTimeSlider.addEventListener("change", lightOffTimeSliderChange)

  loadingPage(true)

  garage.setStateChangeListener(stateUpdate)
  garage.setup()

})
