# Urban-style SUMO/NS-2 mobility trace for 20 nodes
# Initial positions are placed on a 5x4 city grid (x: 0..400, y: 0..300)
# Movements simulate vehicles moving along city blocks.

$node_(0) set X_ 0.0
$node_(0) set Y_ 0.0
$node_(0) set Z_ 0.0
$node_(1) set X_ 100.0
$node_(1) set Y_ 0.0
$node_(1) set Z_ 0.0
$node_(2) set X_ 200.0
$node_(2) set Y_ 0.0
$node_(2) set Z_ 0.0
$node_(3) set X_ 300.0
$node_(3) set Y_ 0.0
$node_(3) set Z_ 0.0
$node_(4) set X_ 400.0
$node_(4) set Y_ 0.0
$node_(4) set Z_ 0.0
$node_(5) set X_ 0.0
$node_(5) set Y_ 100.0
$node_(5) set Z_ 0.0
$node_(6) set X_ 100.0
$node_(6) set Y_ 100.0
$node_(6) set Z_ 0.0
$node_(7) set X_ 200.0
$node_(7) set Y_ 100.0
$node_(7) set Z_ 0.0
$node_(8) set X_ 300.0
$node_(8) set Y_ 100.0
$node_(8) set Z_ 0.0
$node_(9) set X_ 400.0
$node_(9) set Y_ 100.0
$node_(9) set Z_ 0.0
$node_(10) set X_ 0.0
$node_(10) set Y_ 200.0
$node_(10) set Z_ 0.0
$node_(11) set X_ 100.0
$node_(11) set Y_ 200.0
$node_(11) set Z_ 0.0
$node_(12) set X_ 200.0
$node_(12) set Y_ 200.0
$node_(12) set Z_ 0.0
$node_(13) set X_ 300.0
$node_(13) set Y_ 200.0
$node_(13) set Z_ 0.0
$node_(14) set X_ 400.0
$node_(14) set Y_ 200.0
$node_(14) set Z_ 0.0
$node_(15) set X_ 0.0
$node_(15) set Y_ 300.0
$node_(15) set Z_ 0.0
$node_(16) set X_ 100.0
$node_(16) set Y_ 300.0
$node_(16) set Z_ 0.0
$node_(17) set X_ 200.0
$node_(17) set Y_ 300.0
$node_(17) set Z_ 0.0
$node_(18) set X_ 300.0
$node_(18) set Y_ 300.0
$node_(18) set Z_ 0.0
$node_(19) set X_ 400.0
$node_(19) set Y_ 300.0
$node_(19) set Z_ 0.0

$ns_ at 1.0 "$node_(0) setdest 400.0 0.0 12.0"
$ns_ at 2.0 "$node_(1) setdest 100.0 300.0 8.0"
$ns_ at 3.0 "$node_(2) setdest 200.0 300.0 10.0"
$ns_ at 2.0 "$node_(3) setdest 300.0 300.0 10.0"
$ns_ at 1.0 "$node_(4) setdest 0.0 0.0 12.0"
$ns_ at 1.0 "$node_(5) setdest 400.0 100.0 12.0"
$ns_ at 2.0 "$node_(6) setdest 100.0 0.0 10.0"
$ns_ at 4.0 "$node_(7) setdest 400.0 200.0 11.0"
$ns_ at 3.0 "$node_(8) setdest 300.0 0.0 9.0"
$ns_ at 5.0 "$node_(9) setdest 0.0 100.0 10.0"
$ns_ at 1.0 "$node_(10) setdest 400.0 200.0 11.0"
$ns_ at 2.0 "$node_(11) setdest 100.0 300.0 9.0"
$ns_ at 5.0 "$node_(12) setdest 400.0 100.0 10.0"
$ns_ at 3.0 "$node_(13) setdest 300.0 0.0 10.0"
$ns_ at 4.0 "$node_(14) setdest 0.0 200.0 12.0"
$ns_ at 1.0 "$node_(15) setdest 400.0 300.0 11.0"
$ns_ at 3.0 "$node_(16) setdest 100.0 0.0 10.0"
$ns_ at 2.0 "$node_(17) setdest 400.0 100.0 10.0"
$ns_ at 4.0 "$node_(18) setdest 300.0 0.0 10.0"
$ns_ at 1.0 "$node_(19) setdest 0.0 300.0 10.0"

$ns_ at 15.0 "$node_(0) setdest 400.0 300.0 10.0"
$ns_ at 18.0 "$node_(1) setdest 400.0 300.0 10.0"
$ns_ at 20.0 "$node_(2) setdest 0.0 300.0 8.0"
$ns_ at 24.0 "$node_(3) setdest 0.0 0.0 12.0"
$ns_ at 15.0 "$node_(4) setdest 0.0 300.0 8.0"
$ns_ at 20.0 "$node_(5) setdest 400.0 0.0 10.0"
$ns_ at 18.0 "$node_(6) setdest 100.0 300.0 10.0"
$ns_ at 20.0 "$node_(7) setdest 0.0 200.0 10.0"
$ns_ at 15.0 "$node_(8) setdest 300.0 300.0 12.0"
$ns_ at 20.0 "$node_(9) setdest 400.0 100.0 10.0"
$ns_ at 22.0 "$node_(10) setdest 400.0 0.0 10.0"
$ns_ at 18.0 "$node_(11) setdest 100.0 0.0 9.0"
$ns_ at 25.0 "$node_(12) setdest 0.0 100.0 10.0"
$ns_ at 18.0 "$node_(13) setdest 300.0 300.0 11.0"
$ns_ at 22.0 "$node_(14) setdest 400.0 200.0 10.0"
$ns_ at 23.0 "$node_(15) setdest 400.0 0.0 10.0"
$ns_ at 20.0 "$node_(16) setdest 100.0 300.0 10.0"
$ns_ at 24.0 "$node_(17) setdest 0.0 100.0 10.0"
$ns_ at 20.0 "$node_(18) setdest 300.0 300.0 9.0"
$ns_ at 18.0 "$node_(19) setdest 400.0 300.0 10.0"
