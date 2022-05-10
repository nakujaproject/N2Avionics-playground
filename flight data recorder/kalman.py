
def k_filter(u):
    """
    :param u: value we want to filter
    :return: filtered value of v
    """
    u = float(u)

    r = float(0.33)
    h = float(1.00)

    q = float(15)
    p = float(0)
    u_hat = float(0)
    k = float(0)

    # u comes in here. start filtering
    k = (p * h) / (h * p * h + r)  # kalman gain
    u_hat = u_hat + k * (u - h * u_hat)

    # update error covariance
    p = (1 - k * h) * p + q

    # we have a new error covariance. Use it until the next u comes in
    # at the next iteration
    return u_hat
