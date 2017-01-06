extern crate curl;

extern crate regex;
extern crate jni_sys;

use std::error::Error;
use std::io::prelude::*;
use std::fs::File;
use std::path::Path;
use curl::easy::Easy;
use std::time::{Duration, SystemTime};

use regex::Regex;

use std::os::raw::{c_char, c_int};
use std::ffi::CStr;
use std::ptr::null_mut;

/*
fn write_to_file(slice: &[u8]){

    let string_path = String::from("/Users/andrei/Desktop/strings_of_pearls.jpg");

    let path = Path::new(&string_path);
    let display = path.display();

    // Open a file in write-only mode, returns `io::Result<File>`
    let mut file = match File::create(&path) {
        Err(why) => panic!("couldn't create {}: {}",
                           display,
                           why.description()),
        Ok(file) => file,
    };

    match file.write_all(slice) {
        Err(why) => {
            panic!("couldn't write to {}: {}", display,
                   why.description())
        },
        Ok(_) => (),//println!("successfully wrote to {}", display),
    }
}

fn main() {
    println!("Hello, world!");
    //some start url
    //let string_url: &'static str = "https://medpix.nlm.nih.gov/images/full/synpic54731.jpg"; //tiny file
    let string_url: &'static str = "http://services.hanselandpetal.com/selfie%20dogs-488938505.jpg"; //tiny file

    let mut easy = Easy::new();

    let mut buf:Vec<u8> = Vec::new();//String::new().as_bytes();// try!(File::create("/Users/andrei/Desktop/test.jpg"));

    easy.url(string_url).unwrap();

    let mut transfer = easy.transfer();


    transfer.write_function(|data|{
        let mut buffer = Vec::new();
        buffer.extend_from_slice(data);

        buf = buffer.to_vec();
        //buf.extend_from_slice(data);
        write_to_file(&buf);

        Ok(data.len())
    }).unwrap();

    println!("start");
    let now = SystemTime::now();
    transfer.perform().unwrap();
    match now.elapsed(){
        Ok(elapsed) => {
            println!("{} nanos, {} secs", elapsed.subsec_nanos(), elapsed.as_secs());
        },
        Err(e) => {
            println!("{}", e);
        }

    }
    println!("done");
}
*/

/// Constructs a regular expression object. Returns NULL if the pattern is invalid.
#[no_mangle]
pub extern "C" fn rust_regex_create(pattern: *const c_char) -> *mut Regex {
    if pattern.is_null() {
        return null_mut();
    }

    let c_str = unsafe { CStr::from_ptr(pattern) };
    let string = match c_str.to_str() {
        Err(_) => return null_mut(),
        Ok(string) => string,
    };
    let regex = match Regex::new(string) {
        Err(_) => return null_mut(),
        Ok(regex) => Box::new(regex),
    };

    Box::into_raw(regex)
}


/// Destroys a regular expression object previously constructed using `rust_regex_create()`.
#[no_mangle]
pub unsafe extern "C" fn rust_regex_destroy(regex: *mut Regex) {
    drop(Box::from_raw(regex))
}


/// Finds the first index where the regex matches the text. Returns -1 if no match, and a negative
/// number on error.
#[no_mangle]
pub extern "C" fn rust_regex_find(regex: *const Regex, text: *const c_char) -> c_int {
    let regex = match unsafe { regex.as_ref() } {
        None => return -2,
        Some(regex) => regex,
    };

    if text.is_null() {
        return -4;
    }

    let c_str = unsafe { CStr::from_ptr(text) };
    let text = match c_str.to_str() {
        Err(_) => return -3,
        Ok(text) => text,
    };

    match regex.find(text) {
        None => -1,
        Some((start, _)) => start as c_int,
    }
}


/// Expose the JNI interface for android below
#[cfg(target_os="android")]
#[allow(non_snake_case)]
pub mod android {
    use super::*;
    use jni_sys::*;
    use regex::Regex;
    use std::ptr::null_mut;

    #[no_mangle]
    pub unsafe extern "C" fn Java_kennytm_rustsample_RustSampleActivity_regexCreate(env: *mut JNIEnv, _: jclass, java_pattern: jstring) -> jlong {
        let ref java = **env;
        let pattern = (java.GetStringUTFChars)(env, java_pattern, null_mut());
        let regex = rust_regex_create(pattern);
        (java.ReleaseStringUTFChars)(env, java_pattern, pattern);
        regex as jlong
    }

    #[no_mangle]
    pub unsafe extern "C" fn Java_kennytm_rustsample_RustSampleActivity_regexFind(env: *mut JNIEnv, _: jclass, regex_ptr: jlong, java_text: jstring) -> jint {
        let ref java = **env;
        let text = (java.GetStringUTFChars)(env, java_text, null_mut());
        let res = rust_regex_find(regex_ptr as *const Regex, text);
        (java.ReleaseStringUTFChars)(env, java_text, text);
        res
    }

    #[no_mangle]
    pub unsafe extern "C" fn Java_kennytm_rustsample_RustSampleActivity_regexDestroy(_: *mut JNIEnv, _: jclass, regex_ptr: jlong) {
        let regex = regex_ptr as *mut Regex;
        rust_regex_destroy(regex)
    }
}

