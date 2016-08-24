package com.example.andreirybin.nativeapptest.asyncrequests;

import android.os.AsyncTask;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;

/**
 * Created by andreirybin on 8/24/16.
 */

//https://futurestud.io/blog/retrofit-2-how-to-download-files-from-server
public class LargeFileDownloadRequester extends AsyncTask<String, Void, String> {

    private String mBaseUrl;
    private String mUrl;
    private String mExternalFileDir;

    public  LargeFileDownloadRequester(String baseUrl, String url, String externalFileDir) {
        mBaseUrl = baseUrl;
        mUrl = url;
        mExternalFileDir = externalFileDir;
    }

    @Override
    protected String doInBackground(String... strings) {

        final LargeFileDownloaderService largeFileDownloaderService = new Retrofit.Builder()
                .baseUrl(mBaseUrl)
                .build()
                .create(LargeFileDownloaderService.class);
        Call<ResponseBody> call = largeFileDownloaderService.downloadFileWithDynamicUrlSync(mUrl);

        call.enqueue(new Callback<ResponseBody>() {
            @Override
            public void onResponse(Call<ResponseBody> call, Response<ResponseBody> response) {
                if(response.isSuccessful()) {
                    boolean writtenToDisk = writeResponseBodyToDisk(response.body());
                }
            }

            @Override
            public void onFailure(Call<ResponseBody> call, Throwable t) {

            }
        });

        return "urltofile";
    }

    private boolean writeResponseBodyToDisk(ResponseBody body) {
        File fileToWrite = new File(mExternalFileDir + "/somefile.json");
        //these are for debugging purposes
        long fileSize = body.contentLength();
        long fileSizeDownloaded = 0;
        byte[] fileReader = new byte[4096];
        try(InputStream inputStream = body.byteStream();OutputStream outstream = new FileOutputStream(fileToWrite)){
            while(true) {
                int read = inputStream.read(fileReader);
                if(read == -1) {
                    break; //end of file
                }

                outstream.write(fileReader, 0, read);
                fileSizeDownloaded += read;
             }
            outstream.flush();
            inputStream.close();
            outstream.close();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    @Override
    protected void onPostExecute(String s) {
        super.onPostExecute(s);
    }
}
