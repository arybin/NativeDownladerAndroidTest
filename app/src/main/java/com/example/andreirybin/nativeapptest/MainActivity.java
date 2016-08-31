package com.example.andreirybin.nativeapptest;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.andreirybin.nativeapptest.asyncrequests.NativeAsyncNetworkRequest;
import com.squareup.picasso.Callback;
import com.squareup.picasso.MemoryPolicy;
import com.squareup.picasso.NetworkPolicy;
import com.squareup.picasso.Picasso;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    TextView mTextView;
    Button mSendNativeRequest;
    Button mSendPicassoRequest;
    Button mClearbutton;
    String nativeFilePath;
    ImageView mImageView;

    String mExternalStorage;

    private NativeAsyncNetworkRequest mNativeAsyncNetworkRequest;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final String[] urls = {"http://services.hanselandpetal.com/photos/string_of_pearls.jpg", //tiny file
                "http://services.hanselandpetal.com/selfie%20dogs-488938505.jpg", //5mb file
                "https://medpix.nlm.nih.gov/images/full/synpic54731.jpg" //https test
        };

        mExternalStorage = getExternalFilesDir(null).getAbsolutePath() + "/";

        clearCacheFromNative(mExternalStorage);

        final long[] total = {0};
        mTextView = (TextView) findViewById(R.id.sample_text);
        mSendNativeRequest = (Button) findViewById(R.id.send_request_button);
        mSendPicassoRequest = (Button) findViewById(R.id.send_picasso_request_button);
        mImageView = (ImageView) findViewById(R.id.imageViewerDisplay);
        mClearbutton = (Button) findViewById(R.id.clear_images);

        mSendNativeRequest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                mNativeAsyncNetworkRequest = new NativeAsyncNetworkRequest(mExternalStorage, mImageView, mTextView);
                mNativeAsyncNetworkRequest.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR, urls);

            }
        });


        mSendPicassoRequest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(final View view) {
                final long startTime = System.currentTimeMillis();

                Picasso.with(view.getContext())
                        .load(urls[1])
                        .memoryPolicy(MemoryPolicy.NO_CACHE)
                        .networkPolicy(NetworkPolicy.NO_CACHE)
                        .into(mImageView, new Callback() {
                            @Override
                            public void onSuccess() {
                                final long endTime = System.currentTimeMillis();
                                setExecutionTimeOnView(startTime, endTime, total);
                            }

                            @Override
                            public void onError() {

                            }
                        });
            }
        });


        mClearbutton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                clearCacheFromNative(mExternalStorage);
                mImageView.setImageResource(android.R.color.transparent);
                mTextView.setText("Cleared");
            }
        });
    }

    private void setExecutionTimeOnView(long startTime, long endTime, long[] total) {
        total[0] = endTime - startTime;

        StringBuilder sb = new StringBuilder();
        sb.append("Total time: ");
        sb.append(total[0]);
        mTextView.setText(sb.toString());
    }

    private void clearCacheFromNative(String externalStorageLocation) {
        File directory = new File(externalStorageLocation);
        if(directory.exists()) {
            for(File f : directory.listFiles()) {
                if(!f.isDirectory()) {
                    f.delete();
                }
            }
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI(String url, String location);

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("testlibrary");
    }

}
