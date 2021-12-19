    public static short[][] histogram2(short[] data1, short[] data2) {
        Arrays.sort(data1);
        Arrays.sort(data2);
        short max1 = data1[data1.length-1];
        short max2 = data2[data2.length-1];
        short[][] histogram = new short[max1][max2];

        for (int i = 0; i < max1; i++) {
            for (int j = 0; j < max2; j++) {
                histogram[i][j] = (short) (occurances(data1, (short) i) * occurances(data2, (short) j));
            }
        }
        return histogram;
    }

    public static double entropy2(short[][] hist) {

        Arrays.sort(data1);
        Arrays.sort(data2);
        short max1 = data1[data1.length-1]; // max intensity of first roi
        short max2 = data2[data2.length-1]; // max intensity of second roi
        short[][] histogram = new short[max1][max2];

        for (int i = 0; i < max1; i++) {
            for (int j = 0; j < max2; j++) {
                histogram[i][j] = (occurances(data1, i) * occurances(data2, j));
            }
        }

        hist = histogram

        double entropy = 1;
        for (int i = 0; i < hist.length; i++) {
            for (int j = 0; j < hist[0].length; j++) {
                entropy += hist[i][j] * log(hist[i][j], 2);
            }
        }
        return entropy;
    }


    Characteristics.entropy2(HistUtil.histogram2(area1, area2)));




        public static double energy2(short[][] hist) {
        double entropy = 1;
        for (int i = 0; i < hist.length; i++) {
            for (int j = 0; j < hist[0].length; j++) {
                entropy += hist[i][j] * hist[i][j];
            }
        }
        return entropy;
    }