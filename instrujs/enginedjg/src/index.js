/* $Id: index.js, v1.0 2019/11/30 VaderDarth Exp $
 * OpenCPN dashboard_tactics plug-in
 * Licensed under MIT - see distribution.
 */

import '../sass/style.scss'
import { saveParam, getParam, deleteParam } from '../../src/persistence'
import { getLocInfo } from '../../src/location'
import { createGauge } from './gauge'
import { setSkPathFontResizingStyle } from './css'

var gauge = [] // from 1...n gauges, here only [0]!

// Persistent configuration
var uid = ''
var conf = {
    skpath: '',
    title: '',
    unit: '',
    decimals: 1,
    minval: 0,
    maxval: 100,
    theme: '',
    opts: '',
    optn: 0
}
// Functional and environmental
var msie = 0
var bLocalStorage = false
var bCookies = false
var locInfo // not available before window load

// Run-time
var skpath = ''
var titlepath = ''
var unit = ''
var conversion = 100000

function setval(newval) {
    var conval = newval / conversion
    gauge[0].refresh(conval)
}

// Interface to C++, types needs to be set
function setconf(newuid, newskpath, inval, inmin, inmax ) {
    console.log('setconf(): ', newuid, newskpath)
    var nUid = newuid || null
    if (nUid != null)
        uid = nUid
    else {
        console.error('enginedjg.js setconf() - error: null UID')
        return
    }
    if ( nUid == '') {
        if ( uid == '' )
            console.log('enginedjg.js setconf() - warning: empty UID, no existing')
        else
            console.log('enginedjg.js setconf() - warning: empty UID, using existing: ', uid)
    }
    var nSkPath = newskpath || null
    if ( (nSkPath == null) && ( uid == '') ) {
        console.error('enginedjg.js setconf() - error: no uid (no newuid), no newskpath!')
        return
    }
    if ( ( skpath == '' ) && ( uid == '') ) {
        console.error('enginedjg.js setconf() - error: no new UID, no SK path memorized, no UID.')
        return
    }
    if ( ( nSkPath != null) && (nSkPath != '') ) {
        skpath = newskpath
    }
    else {
        if ( uid != '') {
            var nConf = getObj( uid )
            if ( nConf == null ) {
                console.log('enginedjg.js setconf() - warning: new configuration? No conf for: ', uid )
            }
            else {
                conf = nConf
                console.log('enginedjg.js setconf() - For ', uid, ' retrieved: ', conf )
            }
            skpath = conf.skpath
        }
    }
    var arrxsk = skpath.split(".")
    titlepath = "<p>"
    for ( var i = 0; i < (arrxsk.length-1); i++ ) {
        titlepath += arrxsk[i] + "."
    }
    titlepath += "<b>" + arrxsk[arrxsk.length-1] + "</b>"
    document.getElementById('skPath').innerHTML = titlepath
    unit = "[bar]"
    var nMin = inmin || null
    if (nMin == null)
        nMin = 0
    var nMax = inmax || null
    if (nMax == null)
        nMax = 70
    var newval = inval || null
    if (newval == null)
        newval = 700000
    var conval = newval / conversion
    gauge[0].refresh(conval, nMax, nMin, unit )
    if ( uid != '' ) {
        conf.skpath = skpath
        if ( !saveObj( uid, conf ) ) {
            console.error('enginedjg.js setconf() - failed to save for ', uid, ' configuration: ', conf )
        }
    }
    return
}

function regPath(selectedPath) {
    console.log('regPath ', selectedPath )
    setconf( uid, selectedPath, 60 * 100000 )
}

var menu = document.querySelector('.menu')

var alldatapaths = [
    'environment.wind.angleApparent',
    'environment.wind.speedApparent',
    'navigation.courseOverGroundTrue',
    'navigation.speedOverGround',
    'navigation.position.latitude',
    'navigation.position.longitude',
    'propulsion.port.oilPressure',
    'propulsion.port.revolutions',
    'propulsion.port.temperature',
    'propulsion.starboard.oilPressure',
    'propulsion.starboard.revolutions',
    'propulsion.starboard.temperature',
    'battery.empty',
]

var emptypath = [
    'no data available'
]

function setMenu( sortedpath ) {
    var menuul = '<ul id="mi1-u-0" class="menu">'
    var topics = ['','','','','','','','','']
    var submenustart = 0
    for ( var i = 0; i < sortedpath.length; i++ ) {
        var pathel = sortedpath[ i ].split('.')
        for ( var j = 0; j < ( pathel.length - 1); j++ ) {
            if ( pathel[j] != topics[j] ) {
                topics[j] = pathel[j]
                while ( submenustart > j ){
                    menuul += '</ul>'
                    menuul += '</li>'
                    submenustart--         
                }
                submenustart++
                menuul += '<li id="mi1-l-' + i + '-' + j
                menuul += '" class="menu-item menu-item-submenu">'
                menuul += '<button id="mi1-b-' + i + '-' + j
                menuul += '" type="button" class="menu-btn">'
                menuul += '<span id="mi1-s-' + i + '-' + j
                menuul += '" class="menu-text">'
                menuul += pathel[j]
                menuul += '</span></button>'
                menuul += '<ul id="mi1-u-' + i + '-' + j
                menuul += '" class="menu">'           
            }
        }
        menuul += '<li id="mi1-l-' + i + '-' + j + '" class="menu-item">'
        menuul += '<button id="mif-b-'
        menuul += sortedpath[i]
        menuul += '" type="button" class="menu-btn">'
        menuul += '<span id="mif-s-'
        menuul += sortedpath[i]
        menuul += '" class="menu-text">'
        menuul += pathel[j]
        menuul += '</span></button></li>'
    }
    menuul += '</li></ul>'
    document.getElementById('pathMenu').innerHTML = menuul
    document.getElementById('pathMenu').overflow = 'hidden'
    menu = document.querySelector('.menu')
}

var unloadScrollBars = function() {
    document.documentElement.style.overflow = 'hidden' // webkit
    document.body.scroll = "no" // ie
}

window.addEventListener('load',
    function() {
        console.log('loading EngineDJG')
        gauge.push (createGauge('gauge0', 0, 1, '[init]') )
        locInfo = getLocInfo()
        setSkPathFontResizingStyle()
        
        try {
            var sKey  = 'test.enginedjg.js'
            var sTest = 'test'
            localStorage.setItem( sKey, sTest )
            var sComp = localStorage.getItem( sKey )
            localStorage.removeItem( sKey )
            if ( sTest === sComp ) {
                bLocalStorage = true
                console.log('localStorage() support is available, protocol: ', locInfo.protocol)
            }
            else
                console.log('localStorage() support is available but verification failed, protocol: ', locInfo.protocol)
        }
        catch( error ) {
            console.log('localStorage() support not available, protocol: ', locInfo.protocol)
        }
            var cNameTest = 'test-cookie-by-enginedjg-js'
            var cCidTest  = String( Math.floor(Math.random() * Math.floor(999999)) )
            var cValTest  = 'test'
        try {
            if ( saveParam( cNameTest, cCidTest, cValTest ) ) {
                var cValComp = getParam( cNameTest, cCidTest )
                if ( cValComp === cValTest ) {
                    console.log('document.cookie support is available, protocol: ', locInfo.protocol)
                    bCookies = true
                    if ( !deleteParam( cNameTest, cCidTest ) )
                        console.log('document.cookie: warning: cannot delete the test cookie: ', cNameTest, '-', cCidTest)
                }
                else {
                    console.log('document.cookie support is not available (cannot read back), protocol: ', locInfo.protocol)
                }
            }
            else {
                console.log('document.cookie support is not available (cannot save), protocol: ', locInfo.protocol)
            }
        }
        catch( error) {
            console.log('document.cookie support is not available, protocol: ', locInfo.protocol, ' error: ', error)
        }
        if ( !bLocalStorage && !bCookies ) {
            console.error('enginedjg.js: no local storage support available, probably a back-end (IE or WebKit) policy prevents the usage for the protocol: ', locInfo.protocol)
        }
        unloadScrollBars()
        setval(50 * 100000)
        setMenu( emptypath )
    }, false)

/* Menu */
/* Original Copyright (c) 2019 by Ryan Morr (https://codepen.io/ryanmorr/pen/JdOvYR)
   Modified for OpenCPN gauge / display usage */

function showMenu(){
    menu.classList.add('menu-show')
}

function hideMenu(){
    menu.classList.remove('menu-show')
}

function onContextMenu(e){
    e.preventDefault()
    showMenu()
    document.addEventListener('mousedown', onMouseDown )
}

function onMouseDown(e){
    document.removeEventListener('mousedown', onMouseDown)
    e= e.srcElement
    if ( (e.nodeName) === 'BUTTON' || (e.nodeName === 'SPAN') ) {
        if ( e.id !== '' ) {
            var ids = e.id.split( '-' )
            if ( ids[0] == 'mif' ) {
                regPath(ids[2])
                hideMenu()
            }
            else {
                if ( ids[0] == 'mi1' )
                        document.addEventListener('mousedown', onMouseDown )
                else
                    hideMenu()
            }
        }
        else {
            hideMenu()
        }
    }
    else {
        if ( e.id !== '' ) {
            var ids = e.id.split( '-' )
            if ( ids[0] == 'mi1' )
                document.addEventListener('mousedown', onMouseDown )
            else
                hideMenu()
        }
        else
            hideMenu()
    }
}

document.addEventListener('contextmenu', onContextMenu, false)

