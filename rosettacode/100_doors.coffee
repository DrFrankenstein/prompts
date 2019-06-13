doors = new Array 100
doors[idx] = not door for door, idx in doors by pass for pass in [1..100]
console.log "Door #{idx + 1}: " + (if door then "open" else "closed") for door, idx in doors
####################
doors = new Array 100
idx = 1
until (squared = idx * idx) >= 100
  doors[squared] = true
  idx += 1
console.log "Door #{idx + 1}: " + (if door then "open" else "closed") for door, idx in doors
