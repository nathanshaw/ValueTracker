#ifndef __VALUE_TRACKER_DOUBLE_H__
#define __VALUE_TRACKER_DOUBLE_H__

class ValueTrackerDouble {
  public:
      ValueTrackerDouble(double *_val, float lp);
      ValueTrackerDouble(double *_val, float d_rate, uint16_t d_delay, float lp);
      // the update factor will dictate the low_pass filtering amount
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
    double min_recorded;
    double max_recorded;

    /////////// average
    double ravg_val =                      0.0;
    double avg_val  =                      0.0;

    // how many values are in the "average_val" calculator
    uint64_t num_avg_values =              0;
    double delta =                         0.0;

    double *val;
    double last_val;
    double scaled_val;

    // how much will the new value effect the min/max values?
    // should be set to 1.0 under most circumstances
    // how much will the new value effect the rolling average?
    // a 1.0 will result in no averaging
    double low_pass_factor =               0.25;

    // min max adjustment rate needed

    // for decaying the min and max values
    elapsedMillis decay_timer;
    uint16_t decay_delay = 5000;
    double   decay_rate  = 0.005;
};

ValueTrackerDouble::ValueTrackerDouble(double *_val, float lp) {
    last_val = *_val;
    val = _val;
    min_recorded = *_val;
    max_recorded = *_val;
    ravg_val = *_val;
    avg_val = *_val;
    num_avg_values++;
    low_pass_factor = lp;
}

ValueTrackerDouble::ValueTrackerDouble(double *_val, float d_rate, uint16_t d_delay, float lp) {
    decay_rate = d_rate;
    decay_delay = d_delay;
    min_recorded = *_val;
    max_recorded = *_val;
    val = _val;
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
 return scaled_val;
}

void ValueTrackerDouble::update() {
    last_val = *val;
    if (decay_timer > decay_delay) {
        // decrease based on the difference between min and max
        if (max_recorded > min_recorded) {
            Serial.print("decaying values, the difference between values is: ");
            double diff = (max_recorded - min_recorded) * decay_rate;
            Serial.println(diff);
            Serial.print("min/max recorded changed : "); Serial.print(min_recorded);
            Serial.print("\t");Serial.println(max_recorded);
            max_recorded = max_recorded - (diff);
            min_recorded = min_recorded + (diff);
            Serial.print("                         : "); Serial.print(min_recorded);
            Serial.print("\t");Serial.println(max_recorded);
        } else {
            Serial.println("Not decaying values, max_recorded is not less than min_recorded");
        }
        decay_timer = 0;
    }
    // max_recorded update //////////////////////////////
    double mm_lpf = 0.15;
    if (*val > max_recorded) {
        if (low_pass_factor != 1.0) {
            // TODO - i think i need a different value for the min/ax low_padd_factor
            // Serial.print("max_recorded changed from x -> x:\t");Serial.print(max_recorded);
            max_recorded = (max_recorded * (1.0 - mm_lpf)) + (*val * mm_lpf);
            // Serial.print("\t->\t");Serial.println(max_recorded);
            *val = max_recorded;
            scaled_val = 1.0;
        } else {
            max_recorded = *val;
            scaled_val = 1.0;
        }
    }
    else if (*val < min_recorded) {
        // min_value update /////////////////////////////////
        if (low_pass_factor != 1.0) {
            // Serial.print("min_recorded changed from x -> x:\t");Serial.print(min_recorded);
            min_recorded = (min_recorded * (1.0 - mm_lpf)) + (*val * mm_lpf);
            // Serial.print("\t->\t");Serial.println(min_recorded);
            *val = min_recorded;
            scaled_val = 0.0;
        } else {
            min_recorded = *val;
            scaled_val = 0.0;
        }
    } else {
        scaled_val = (*val - min_recorded) / max_recorded;
        // Serial.print("scaled value is : ");Serial.println(scaled_val);
        // Serial.print("min/max is :\t");Serial.print(min_recorded);Serial.print("\t");
        // Serial.println(max_recorded);
        if (scaled_val < 0.0) {
            scaled_val = 0.0;
        } else if (scaled_val > 1.0) {
            scaled_val = 1.0;
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

#endif // __VALUE_TRACKER_DOUBLE_H__
