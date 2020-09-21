#ifndef __VALUE_TRACKER_FLOAT_H__
#define __VALUE_TRACKER_FLOAT_H__

class ValueTrackerFloat() {
  public:
      ValueTrackerFloat(float &_val);
      // the update factor will dictate the low_pass filtering amount
      void setUpdateFactor(float _factor);
      void update();

      float getPosDelta();

      float getMin(bool reset);
      float getMin(){getMin(false);};

      float getMax(bool reset);
      float getMax(){getMax(false);};

      float getAvg(bool reset);
      float getAvg(){getAvg(false)};

      float getRAvg(bool reset);
      float getRAvg(){getRAvg(false)};

      float getScaled();

  private:
    float min_recorded =                   99999.9;
    float max_recorded =                   -99999.9;

    /////////// average
    float ravg_val =                      0.0;
    float totals =                         0.0;

    // how many values are in the "average_val" calculator
    uint64_t num_values =                   0;
    float pos_delta =                      0.0;

    float &val;
    float last_val;

    // how much will the new value effect the min/max values?
    // should be set to 1.0 under most circumstances
    float ravg_factor =                         1.0;
}

ValueTrackerFloat(double &val) {
    last_val = _val;
    val = _val;
    min_recorded = _val;
    max_recorded = _val;
    ravg_val = _val;
    totals = _val;
    num_values++;
};

double getScaled() {
 return (val - min_recorded) / max_recorded;
}

void ValueTrackerFloat::update() {
    // max_recorded update //////////////////////////////
    if (val > max_recorded) {
        if (factor != 1.0) {
            max_recorded = (max_recorded * (1.0 - factor)) + (val * factor);
        } else {
            max_recorded = val;
        }
    }

    // min_value update /////////////////////////////////
    if (val < min_recorded) {
        if (factor != 1.0) {
            min_recorded = (min_recorded * (1.0 - factor)) + (val * factor);
        } else {
            min_recorded = val;
        }
    }
    // average_val update ///////////////////////////////
    // take the running average and multiple it against now many readings there have
    // been thus far, then add current value and divide by total number of readings
    // including current reading.
    ravg_val = (last_val + val) * ravg_factor;

    total += val;

    if (average_active || rolling_average_active == true) {
        num_values++;
    }

    // pos delta update
    if (pos_delta_active == true) {
        delta = val - last_val;
    }
    last_val = val;
}

double ValueTrackerFloat::getAverage() {
    return total / num_values;
}

double ValueTrackerFloat::resetAverage() {
    total = 0;

}

double ValueTrackerFloat::getPosDelta() {
    if (delta > 0.0) {
        return delta;
    }
    return 0.0;
}

void ValueTrackerFloat::setUpdateFactor(double _factor) {
    factor = _factor;
}

#endif // __VALUE_TRACKER_DOUBLE_H__
