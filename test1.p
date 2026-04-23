(function (main)
  (block
    (set n (scan))
    (set sum 0)
    (for
      (set i 0)
      (< i n)
      (set i (+ i 1))
      (set sum (+ sum i)))
    (print sum)))
