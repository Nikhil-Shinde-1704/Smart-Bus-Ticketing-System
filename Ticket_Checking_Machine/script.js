let machineOn = false; // Set the initial state to off

let randomNames = [ "Nikhil Shinde",
"Aniket Mane",
"Prathmesh Bale",
"Sarvesh Pawaskar",
"Gaurav Gajbare",
"Arpan Yeole",
"Avdhoot Bhilare"];
let randomUIDs = ["u287364hf",
"u98347sjd",
"u65d4g7df",
"u8493ghfd",
"u54h7g9df",
"u7643fghd",
"u239dgh4f",
"u0945h9gd",
"u9824h7gd",
"u763gf43h",
"u32fg47hd",
"u5498gh43",
"u879dfg3h",
"u654h87fg",
"u23gf47hd",
"u875hgf34",
"u2394gh5d",
"u32gf487h",
"u567gf34h",
"u2394h7gd"];
let pickupLocations = ["Akurdi Railway Station",
"Akurdi Police Station",
"Sambhaji Chowk",
"Bijalinagar Corner",
"Chintamani Pravesh Dwaar",
"Chinchwade Farm",
"Munjaba Vasahat",
"Chinchwade Nagar Corner",
"Gokhale Park Walhekarwadi Road",
"Chaphekar Chowk",
"Chinchwad Gaon",];
let dropLocations = ["Akurdi Railway Station",
"Akurdi Police Station",
"Sambhaji Chowk",
"Bijalinagar Corner",
"Chintamani Pravesh Dwaar",
"Chinchwade Farm",
"Munjaba Vasahat",
"Chinchwade Nagar Corner",
"Gokhale Park Walhekarwadi Road",
"Chaphekar Chowk",
"Chinchwad Gaon",];

function toggleMachine() {
    machineOn = !machineOn;
    updateScreen();
}

function calculateDistance(pickup, drop) {
    // Simulate distance calculation based on the index difference between pickup and drop locations
    return Math.abs(pickupLocations.indexOf(pickup) - dropLocations.indexOf(drop));
}

function calculateFare(distance) {
    // Simulate fare calculation based on a fixed rate per unit distance
    const fareRatePerUnitDistance = 5; // $5 per unit distance (this can be adjusted)
    return distance * fareRatePerUnitDistance;
}

function updateScreen() {
    const passengerDisplay = document.getElementById('screen');
    if (machineOn) {
        // Generate random indices to select a name, UID, pickup, and drop locations from the arrays
        const randomIndex = Math.floor(Math.random() * randomNames.length);
        const randomName = randomNames[randomIndex];
        const randomUID = randomUIDs[randomIndex];
        const randomPickupLocation = pickupLocations[Math.floor(Math.random() * pickupLocations.length)];
        const randomDropLocation = dropLocations[Math.floor(Math.random() * dropLocations.length)];

        const distance = calculateDistance(randomPickupLocation, randomDropLocation);
        const fare = calculateFare(distance);

        passengerDisplay.innerHTML = `Name: ${randomName}<br>UID: ${randomUID}<br>Pickup Location: ${randomPickupLocation}<br>Drop Location: ${randomDropLocation}<br>Fare: $${fare}<br>`;
    } else {
        passengerDisplay.innerHTML = "Machine Off";
    }
}

document.querySelector('.rfid-card').addEventListener('mouseover', updateScreen);
document.querySelector('.rfid-card').addEventListener('mouseout', () => {
    document.getElementById('screen').innerHTML = ''; // Clear passenger information when mouseout
});
