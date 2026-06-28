class MeanPowerFilter {
   public:
    MeanPowerFilter() {
        _data = new std::vector<double>();
    }

    ~MeanPowerFilter() {
        if (_data) delete _data;
    }

    virtual void reset() {
        // double last = _data->empty() ? 0 : _data->back();
        _data->clear();
    }

    virtual void add(double value) {
        _data->push_back(value);
    }

    virtual double mean(bool do_reset = false) {
        double sum = 0;

        if (_data->size() > 0) {
            for (unsigned char i = 0; i < _data->size(); i++) {
                double current = _data->at(i);

                sum += current;
            }

            sum /= (_data->size());
        }

        if (do_reset) reset();

        return sum;
    }

    virtual unsigned char count() {
        return _data->size();
    }

   private:
    std::vector<double> *_data;
};