/*
  An object representing a remote, Wi-Fi control IoT garage.
  Features include:
     Ability to open/close the door
     Ability to turn on/off the light
     Ability to enable/disable door Auto-Close feature
     Ability to configure time used for door Auto-Close feature
     Ability to configure time used for light Auto-Off feature
*/

var myParticleAccessToken = "1a357255a7ae96b19fddcb4c1a0eda21a5b5ac5e"
var myDeviceId = "500020000751373238323937"
var topic = "cse222garageDoor"

function newGarageEvent(objectContainingData) {
      let data = JSON.parse(objectContainingData.data)
      garage.doorPowered = data.doorState;
      garage.autoCloseEnabled = data.isAutoCloseEnabled

      garage.doorAutoCloseTime = data.autoCloseTime / 1000

      garage.haveFault = data.haveFault
      garage.lightPowered = data.lightOn;
      garage.lightBrightness = data.lightBrightness;
      garage.lightAutoOffTime = data.lightAutoOffTime / 1000;
      console.log(data);
      // publish the state to any listeners
      garage.stateChange()
    }

const doorStatus = {
    open: "Open",
    close: "Closed",
    opening: "Opening",
    closing: "Closing",
    should_be_closing: "Stopped - Should Be Closing",
    should_be_opening: "Stopped - Should Be Opening"
  }

//garage object - saving all information
  var garage = {
      doorPowered: doorStatus.close,
      lightPowered: false,
      lightBrightness: 100,
      autoCloseEnabled: false,
      doorAutoCloseTime: 30,
      lightAutoOffTime: 30,
      haveFault: false,

      stateChangeListener: null,

      particle: null,

      //funcion represents door button hit
      setDoorPowered: function() {
        //change the door state based on the current state
        if (this.doorPowered == doorStatus.open) {
          this.doorPowered = doorStatus.closing
        }
        else if (this.doorPowered == doorStatus.close) {
          this.doorPowered = doorStatus.opening
        }
        else if (this.doorPowered == doorStatus.opening) {
          this.doorPowered = doorStatus.should_be_closing
        }
        else if (this.doorPowered == doorStatus.closing) {
          this.doorPowered = doorStatus.should_be_opening
        }
        else if (this.doorPowered == doorStatus.should_be_opening) {
          this.doorPowered = doorStatus.opening
        }
        else if (this.doorPowered == doorStatus.should_be_closing) {
          this.doorPowered = doorStatus.closing
        }

        var functionData = {
          deviceId:myDeviceId,
          name: "doorButtonPress",
          argument: "",
          auth: myParticleAccessToken
       }

          // Include functions to provide details about the process.
        function onSuccess(e) { console.log("setDoorPowered call success") }
        function onFailure(e) { console.log("setDoorPowered call failed")
                              console.dir(e) }
        particle.callFunction(functionData).then(onSuccess,onFailure)
      },

      //function called when light button is hit
      setLightPowered: function() {
        var status;
        if (this.lightPowered) {
          this.lightPowered = false
          status = "false";
        } else {
          this.lightPowered = true
          status = "true";
        }

        var functionData = {
          deviceId:myDeviceId,
          name: "lightButtonPress",
          argument: "",
          auth: myParticleAccessToken
       }

          // Include functions to provide details about the process.
        function onSuccess(e) { console.log("setLightPowered call success") }
        function onFailure(e) { console.log("setLightPowered call failed")
                              console.dir(e) }
        particle.callFunction(functionData).then(onSuccess,onFailure)
      },

      //function to change light brightness
      setLightBrightness: function(brightness) {
        this.lightBrightness = brightness

        var functionData = {
          deviceId:myDeviceId,
          name: "lightBrightnessChange",
          argument: brightness,
          auth: myParticleAccessToken
       }

          // Include functions to provide details about the process.
        function onSuccess(e) { console.log("setLightBrightness call success") }
        function onFailure(e) { console.log("setLightBrightness call failed")
                              console.dir(e) }
        particle.callFunction(functionData).then(onSuccess,onFailure)
      },

      //enable/disable auto-close feature
      setAutoCloseEnabled: function() {
        var status
        if (this.autoCloseEnabled == false) {
          this.autoCloseEnabled = true
          status = "true"

        } else {
          this.autoCloseEnabled = false
          status = "false"
        }

        var functionData = {
          deviceId:myDeviceId,
          name: "setAutoCloseEnabled",
          argument: status,
          auth: myParticleAccessToken
       }

          // Include functions to provide details about the process.
        function onSuccess(e) { console.log("setAutoCloseEnabled call success") }
        function onFailure(e) { console.log("setAutoCloseEnabled call failed")
                              console.dir(e) }
        particle.callFunction(functionData).then(onSuccess,onFailure)

      },

      //change auto-close time
      setDoorAutoCloseTime: function(time) {
        this.doorAutoCloseTime = time
        let timeString = String(time * 1000)      //convert second to millisecond and pass to function
        var functionData = {
          deviceId:myDeviceId,
          name: "setAutoCloseTime",
          argument: timeString,
          auth: myParticleAccessToken
       }

          // Include functions to provide details about the process.
        function onSuccess(e) { console.log("setAutoCloseTime call success") }
        function onFailure(e) { console.log("setAutoCloseTime call failed")
                              console.dir(e) }
        particle.callFunction(functionData).then(onSuccess,onFailure)

      },

      //change auto-off time of light
      setLightAutoOffTime: function(time) {
        this.lightAutoOffTime = time
        let timeString = String(time * 1000)

        var functionData = {
          deviceId:myDeviceId,
          name: "setLightAutoOffTime",
          argument: timeString,
          auth: myParticleAccessToken
       }

          // Include functions to provide details about the process.
        function onSuccess(e) { console.log("setLightAutoOffTime call success") }
        function onFailure(e) { console.log("setLightAutoOffTime call failed")
                              console.dir(e) }
        particle.callFunction(functionData).then(onSuccess,onFailure)

      },

      setStateChangeListener: function(aListener) {
        this.stateChangeListener = aListener
      },

      stateChange: function() {
        if (this.stateChangeListener) {
          var state = { doorPowered: this.doorPowered,
                        autoCloseEnabled: this.autoCloseEnabled,
                        doorAutoCloseTime: this.doorAutoCloseTime,
                        haveFault: this.haveFault,
                        lightPowered: this.lightPowered,
                        lightBrightness: this.lightBrightness,
                        lightAutoOffTime: this.lightAutoOffTime};
        this.stateChangeListener(state);
        }
      },

      setup: function() {
      // Create a particle object
        particle = new Particle();

        // Get ready to subscribe to the event stream
        function onSuccess(stream) {
          console.log("getEventStream success")
          stream.on('event', newGarageEvent)

          var functionData = {
            deviceId: myDeviceId,
            name: "publishState",
            argument: "",
            auth: myParticleAccessToken
          }
          function onSuccess2(e) { console.log("publish call success") }
          function onFailure2(e) { console.log("publish call failed")
                                console.dir(e) }

          particle.callFunction(functionData).then(onSuccess2, onFailure2);
        }
        function onFailure(e) { console.log("getEventStream call failed")
                                console.dir(e) }
        // Subscribe to the stream
        particle.getEventStream( { name: topic, auth: myParticleAccessToken, deviceId: 'mine' }).then(onSuccess, onFailure)
    }
  }
