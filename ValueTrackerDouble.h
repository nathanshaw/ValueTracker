#ifndef __VALUE_TRACKER_DOUBLE_H__
#define __VALUE_TRACKER_DOUBLE_H__

class ValueTrackerDouble {
  public:
      ValueTrackerDouble(double *_val, double lp);
      // the update factor will dictate the low_pass filtering amount
      void setUpdateFactor(double _factor);
      void update();
      void reset();

      double getPosDelta();
      double getNegDelta();
      double getDelta(){return delta;};

      double getMin(bool reset);
      double getMin(){return getMin(false);};

      double getMax(bool reset);
      double getMax(){return getMax(false);};

      double getAvg(bool reset);
      double getAvg(){return getAvg(false);};

      double getRAvg(bool reset);
      double getRAvg(){return getRAvg(false);};

      double getLastVal(){return last_val;};

      double getScaled();

      void printStats();
      void print(){printStats();};

  private:
    double min_recorded =                   99999.9;
    double max_recorded =                   -99999.9;

    /////////// average
    double ravg_val =                      0.0;
    double avg_val  =                      0.0;

    // how many values are in the "average_val" calculator
    uint64_t num_avg_values =              0;
    double delta =                         0.0;

    double *val;
    double last_val;

    // how much will the new value effect the min/max values?
    // should be set to 1.0 under most circumstances
    double ravg_factor =                   1.0;
    // how much will the new value effect the rolling average?
    // a 1.0 will result in no averaging
    double low_pass_factor =               0.25;
};

ValueTrackerDouble::ValueTrackerDouble(double *_val, double lp) {
    last_val = *_val;
    val = _val;
    min_recorded = *_val;
    max_recorded = *_val;
    ravg_val = *_val;
    avg_val = *_val;
    num_avg_values++;
    low_pass_factor = lp;
}

void ValueTrackerDouble::reset() {
    Serial.println("resetting the averages and min/max values for the value tracker double");
    min_recorded = *val;
    max_recorded = *val;
    num_avg_values = 1;
    avg_val = *val;
    ravg_val = *val;
}

void ValueTrackerDouble::printStats() {
    Serial.println("---------- printing value tracker values ----------------");
    Serial.print("value   : "); Serial.println(*val);
    Serial.print("min/max : ");Serial.print(min_recorded);
    Serial.print("\t");Serial.println(max_recorded);
    Serial.print("avg/ravg: ");Serial.print(avg_val); 
    Serial.print("\t");Serial.println(ravg_val);
    Serial.print("numvals : ");Serial.println((long)num_avg_values);
    Serial.print("delta   : ");Serial.println(delta);
}

double ValueTrackerDouble::getScaled() {
 return (*val - min_recorded) / max_recorded;
}

void ValueTrackerDouble::update() {
    // max_recorded update //////////////////////////////
    if (*val > max_recorded) {
        if (ravg_factor != 1.0) {
            max_recorded = (max_recorded * (1.0 - ravg_factor)) + (*val * ravg_factor);
        } else {
            max_recorded = *val;
        }
    }
    // min_value update /////////////////////////////////
    if (*val < min_recorded) {
        if (ravg_factor != 1.0) {
            min_recorded = (min_recorded * (1.0 - ravg_factor)) + (*val * ravg_factor);
        } else {
            min_recorded = *val;
        }
    }
    // average_val update ///////////////////////////////
    // take the running average and multiple it against now many readings there have
    // been thus far, then add current value and divide by total number of readings
    // including current reading.
    avg_val = ((num_avg_values * avg_val) + *val) / (num_avg_values + 1);
    num_avg_values++;
 
    ////////////// Rolling Average /////////////////////
    // the rolling average is simple low pass filter 
    ravg_val = (last_val * (1.0 - low_pass_factor)) + (*val * low_pass_factor);

    /////////////// Delta //////////////////////////////
    delta = *val - last_val;
    last_val = *val;
}

double ValueTrackerDouble::getMin(bool reset) {
    double m = min_recorded;
    if (reset == true) {
        min_recorded = *val;
    }
    return m;
}

double ValueTrackerDouble::getMax(bool reset) {
    double m = max_recorded;
    if (reset == true) {
        max_recorded = *val;
    }
    return m;
}

double ValueTrackerDouble::getAvg(bool reset) {
    double a = avg_val;
    if (reset == true) {
        avg_val = *val;
        num_avg_values = 1;
    }
    return a;
}

double ValueTrackerDouble::getRAvg(bool reset) {
    double a = ravg_val;
    if (reset == true) {
        ravg_val = *val;
    }
    return a;
}

double ValueTrackerDouble::getPosDelta() {
    if (delta > 0.0) {
        return delta;
    }
    return 0.0;
}

double ValueTrackerDouble::getNegDelta() {
    if (delta < 0.0) {
        return delta;
    }
    return 0.0;
}

void ValueTrackerDouble::setUpdateFactor(double _factor) {
    ravg_factor = _factor;
}

#endif // __VALUE_TRACKER_DOUBLE_H__
