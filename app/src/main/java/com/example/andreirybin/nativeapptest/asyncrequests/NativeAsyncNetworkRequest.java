package com.example.andreirybin.nativeapptest.asyncrequests;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;

/**
 * Created by andreirybin on 8/24/16.
 */

public class NativeAsyncNetworkRequest extends AsyncTask<String, Void, String> {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private String mLocationOnDisk;
    private ImageView mViewToPopulate;
    private TextView mTitle;

    private long mStartTime;
    private long mEndTime;

    public NativeAsyncNetworkRequest(String externalStorage, ImageView imageView, TextView title) {
        mLocationOnDisk = externalStorage;
        mViewToPopulate = imageView;
        mTitle = title;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String downloadLocationFromNative(String url, String location);


    @Override
    protected String doInBackground(String... strings) {
        String url = strings[0];
        mStartTime = System.currentTimeMillis();
        return downloadLocationFromNative(url, mLocationOnDisk);
    }

    @Override
    protected void onPostExecute(String s) {

        //this is going to return once the location comes back
        super.onPostExecute(s);

        File imgFile = new File(s);
        if(imgFile.exists()) {
            //TODO this throws out of memory exception for large files, need to fix it
            Bitmap bitmap = BitmapFactory.decodeFile(imgFile.getAbsolutePath());
            mViewToPopulate.setImageBitmap(bitmap);
        }
        mEndTime = System.currentTimeMillis();
        String sb = "Total time: " +
                (mEndTime - mStartTime);
        mTitle.setText(sb);
    }
}
