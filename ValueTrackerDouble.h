#ifndef __VALUE_TRACKER_DOUBLE_H__
#define __VALUE_TRACKER_DOUBLE_H__

class ValueTrackerDouble() {
  public:
      ValueTrackerDouble(double &_val);
      // the update factor will dictate the low_pass filtering amount
      void setUpdateFactor(double _factor);
      void update();

      double getPosDelta();

      double getMin(bool reset);
      double getMin(){getMin(false);};

      double getMax(bool reset);
      double getMax(){getMax(false);};

      double getAvg(bool reset);
      double getAvg(){getAvg(false)};

      double getRAvg(bool reset);
      double getRAvg(){getRAvg(false)};

      double getScaled();

  private:
    double min_recorded =                   99999.9;
    double max_recorded =                   -99999.9;

    /////////// average
    double ravg_val =                      0.0;
    double totals =                         0.0;

    // how many values are in the "average_val" calculator
    uint64_t num_values =                   0;
    double pos_delta =                      0.0;

    double &val;
    double last_val;

    // how much will the new value effect the min/max values?
    // should be set to 1.0 under most circumstances
    double ravg_factor =                         1.0;
}

ValueTrackerDouble(double &val) {
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

void ValueTrackerDouble::update() {
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

double ValueTrackerDouble::getAverage() {
    return total / num_values;
}

double ValueTrackerDouble::resetAverage() {
    total = 0;

}

double ValueTrackerDouble::getPosDelta() {
    if (delta > 0.0) {
        return delta;
    }
    return 0.0;
}

void ValueTrackerDouble::setUpdateFactor(double _factor) {
    factor = _factor;
}

#endif // __VALUE_TRACKER_DOUBLE_H__
