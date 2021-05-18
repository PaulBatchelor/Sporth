#!/bin/bash

set -o pipefail

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
   MD5=md5sum
else
   MD5=md5
fi 

test_example() {
    ./sporth -b raw examples/$1 | $MD5 | grep -q $2

    if [ $? == 0 ]; then
        echo "$1 PASSED"
    else
        echo "$1 FAILED. Expected $2. Got `./sporth -b raw examples/$1 | $MD5`"
    fi
}

test_example biscale.sp 78165070f45ee7cd1f390077286b8b19
test_example blsquare.sp 4a1680392100440597313a684a9c773c
test_example bltriangle.sp b671058e7478cd3af296bc45b60d893a
# test_example branch.sp ee6fcdd92cfb6539e2a323fb58804523
# test_example buthp.sp 2e27fa59191775bd1c212fdd1df03f77
# test_example butlp.sp 34480c097eea3b61b296fe4bd97d6efb
# test_example comb.sp d41d8cd98f00b204e9800998ecf8427e
test_example count.sp ac89d8101f12dc047dbb0ff39d80d473
test_example crossfade.sp a66b3eefa1c442d83dec94bf245b9e99
test_example dialtone.sp 4ec984e80addbbef6a77e99d1029a726
test_example dmetro.sp e70125cbad633c69a7633eb6b76e644c
test_example dtrig.sp 692aed7f572297655d3574d5caf8bc39
test_example expon.sp 4f9e12e6f33983c79e5713b0a63ef9f5
# test_example f.sp d41d8cd98f00b204e9800998ecf8427e
# test_example filelist.txt d41d8cd98f00b204e9800998ecf8427e
test_example fm.sp 0afb84166f8edc2079992b8bfe52eb4d
# test_example fof.sp d41d8cd98f00b204e9800998ecf8427e
# test_example fog.sp d41d8cd98f00b204e9800998ecf8427e
# test_example gain.c d41d8cd98f00b204e9800998ecf8427e
# test_example gbuzz.sp d41d8cd98f00b204e9800998ecf8427e
# test_example gen_line.sp d41d8cd98f00b204e9800998ecf8427e
# test_example gen_sinesum.sp d41d8cd98f00b204e9800998ecf8427e
# test_example gen_sporth.sp d41d8cd98f00b204e9800998ecf8427e
# test_example imp.wav d41d8cd98f00b204e9800998ecf8427e
# test_example jitter.sp d41d8cd98f00b204e9800998ecf8427e
# test_example kona.sp d41d8cd98f00b204e9800998ecf8427e
# test_example ladspa.sp d41d8cd98f00b204e9800998ecf8427e
test_example line.sp 662e07e549915f701ade5aa62378a5d8
# test_example load.sp d41d8cd98f00b204e9800998ecf8427e
# test_example lpc.sp d41d8cd98f00b204e9800998ecf8427e
test_example lsys.sp f51337b1e0f8eaf13c4ec42125c1696a
# test_example maygate.sp 89dfb1cdea0dfa48ad7ebfc6ea55be6a
# test_example metro.sp 9483ffc6ab191ea5fdfe4628f75eeb3c
# test_example mincerbuf.sp d41d8cd98f00b204e9800998ecf8427e
# test_example mode.sp a4f28db6f8904805a2ba711bd412d329
# test_example moogladder.sp d41d8cd98f00b204e9800998ecf8427e
# test_example nsmp.sp d41d8cd98f00b204e9800998ecf8427e
# test_example oneart.ini d41d8cd98f00b204e9800998ecf8427e
# test_example oneart.wav d41d8cd98f00b204e9800998ecf8427e
test_example osc.sp fd628a3f8c64dc16e76b93a343f71db1
# test_example padsynth.sp d41d8cd98f00b204e9800998ecf8427e
# test_example pan.sp d41d8cd98f00b204e9800998ecf8427e
# test_example pareq.sp d41d8cd98f00b204e9800998ecf8427e
# test_example paulstretch.sp d41d8cd98f00b204e9800998ecf8427e
# test_example pdhalf.sp d41d8cd98f00b204e9800998ecf8427e
# test_example phaser.sp d41d8cd98f00b204e9800998ecf8427e
# test_example pluck.sp d41d8cd98f00b204e9800998ecf8427e
# test_example pluck_simple.sp d41d8cd98f00b204e9800998ecf8427e
# test_example pluck_stereo.sp d41d8cd98f00b204e9800998ecf8427e
# test_example port.sp d41d8cd98f00b204e9800998ecf8427e
test_example portamento.sp ff1016b4b171d9f9e8edf2ee2dbc6494
# test_example posc3.sp d41d8cd98f00b204e9800998ecf8427e
# test_example print.sp cf10c3426d515f229c8b656ca3594583
# test_example prop.sp d41d8cd98f00b204e9800998ecf8427e
# test_example pshift.sp d41d8cd98f00b204e9800998ecf8427e
# test_example ptrack.sp d41d8cd98f00b204e9800998ecf8427e
# test_example randh.sp 22b9e433b644127dc87bca480c48532d
# test_example randi.sp 4d273adc67b2b19864e44a5311cfadac
# test_example reverse.sp d41d8cd98f00b204e9800998ecf8427e
test_example revsc.sp b62444d6bd4b74a23d3670b89aed3d13
# test_example rms.sp d41d8cd98f00b204e9800998ecf8427e
# test_example rpt.sp d41d8cd98f00b204e9800998ecf8427e
# test_example samphold.sp ea0dc29144017e00f91e9d8dccddc2dd
test_example seq.sp 72a49d6f21d27d6f0cb56466c3b624b2
# test_example slicer.sp d41d8cd98f00b204e9800998ecf8427e
# test_example slist.sp d41d8cd98f00b204e9800998ecf8427e
# test_example smoothdelay.sp d41d8cd98f00b204e9800998ecf8427e
# test_example sndwarp.sp d41d8cd98f00b204e9800998ecf8427e
# test_example sporth_poly.sp d41d8cd98f00b204e9800998ecf8427e
# test_example streson.sp d41d8cd98f00b204e9800998ecf8427e
# test_example t.db 444ec79bcbdc0f22e9b5e652d61b90af
# test_example tabread.sp d41d8cd98f00b204e9800998ecf8427e
# test_example talkbox.sp d41d8cd98f00b204e9800998ecf8427e
test_example tdiv.sp 09f639a51713932b777f13f8fbae6098
test_example tenv.sp 7781fd98a2aeb82b8aca39a193d5b7d8
test_example tenv2.sp 09e845bf027c55c1d85c51cccc034c34
# test_example timer.sp 3790ca7e1826f12bcc8ba31449827d4d
test_example tog.sp 3e30a3dbc5a53fd07b41fc3c1a26c78b
# test_example tone.sp d41d8cd98f00b204e9800998ecf8427e
# test_example tphasor.sp d41d8cd98f00b204e9800998ecf8427e
# test_example trand.sp 5c3fdc35dbbf28e9cf86a6abf8ff3002
test_example tri.sp 6a2eb43579fa2f915a91d52e55a8f5ea
test_example tseq.sp 4ec7ae3cd02253dbb08d2c25f694ead7
# test_example variables.sp e8263eec62ec215283c74cfa0ca85cfa
# test_example vdelay.sp d41d8cd98f00b204e9800998ecf8427e
# test_example waveset.sp d41d8cd98f00b204e9800998ecf8427e
# test_example zitarev.sp d41d8cd98f00b204e9800998ecf8427e
# test_example zrev.sp e4157471856c2499ba5ddf5eaa65fd4c
